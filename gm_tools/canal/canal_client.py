#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Canal Client - MySQL镜像数据库同步客户端
DNF GM工具 - 数据同步服务
Week 3 基础设施配置
生成日期: 2025-11-09

功能：
1. 连接Canal Server订阅binlog事件
2. 解析binlog事件（INSERT/UPDATE/DELETE）
3. 将变更写入GM镜像数据库
4. 监控同步延迟和错误
"""

import os
import sys
import time
import json
import logging
from datetime import datetime
from typing import Dict, List, Optional

# Canal Python Client
# pip install canal-python
from canal.client import Client
from canal.protocol import EntryProtocol_pb2, CanalProtocol_pb2

# MySQL连接
# pip install pymysql
import pymysql
from pymysql import Connection

# ========================================
# 配置
# ========================================

# Canal Server配置
CANAL_HOST = os.getenv('CANAL_HOST', 'canal')
CANAL_PORT = int(os.getenv('CANAL_PORT', 11111))
CANAL_DESTINATION = os.getenv('CANAL_DESTINATION', 'dnf_gm_sync')
CANAL_CLIENT_ID = os.getenv('CANAL_CLIENT_ID', 'gm_sync_client_1')

# 目标数据库配置（GM镜像库）
TARGET_DB_HOST = os.getenv('TARGET_DB_HOST', 'gm-mirror-db')
TARGET_DB_PORT = int(os.getenv('TARGET_DB_PORT', 3306))
TARGET_DB_USER = os.getenv('TARGET_DB_USER', 'root')
TARGET_DB_PASSWORD = os.getenv('TARGET_DB_PASSWORD', '')

# 数据库映射（生产库名 -> 镜像库名）
DB_MAPPING = {
    'taiwan_cain': 'taiwan_cain_mirror',
    'taiwan_login': 'taiwan_login_mirror'
}

# 批量提交配置
BATCH_SIZE = 100  # 每批处理的事件数
BATCH_TIMEOUT = 1  # 批量提交超时时间（秒）

# 日志配置
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('/var/log/gm_tools/canal_client.log'),
        logging.StreamHandler(sys.stdout)
    ]
)
logger = logging.getLogger('canal_client')

# ========================================
# CanalSyncClient 类
# ========================================

class CanalSyncClient:
    """Canal同步客户端"""

    def __init__(self):
        self.canal_client: Optional[Client] = None
        self.db_conn: Optional[Connection] = None
        self.running = False

        # 统计信息
        self.stats = {
            'total_events': 0,
            'insert_count': 0,
            'update_count': 0,
            'delete_count': 0,
            'error_count': 0,
            'last_sync_time': None
        }

    def connect_canal(self):
        """连接Canal Server"""
        try:
            self.canal_client = Client()
            self.canal_client.connect(host=CANAL_HOST, port=CANAL_PORT)
            self.canal_client.check_valid(username=b'', password=b'')
            self.canal_client.subscribe(
                client_id=CANAL_CLIENT_ID.encode('utf-8'),
                destination=CANAL_DESTINATION.encode('utf-8'),
                filter=b''  # 使用instance.properties中配置的filter
            )
            logger.info(f"Connected to Canal Server: {CANAL_HOST}:{CANAL_PORT}")
            return True
        except Exception as e:
            logger.error(f"Failed to connect to Canal: {e}")
            return False

    def connect_db(self):
        """连接目标数据库"""
        try:
            self.db_conn = pymysql.connect(
                host=TARGET_DB_HOST,
                port=TARGET_DB_PORT,
                user=TARGET_DB_USER,
                password=TARGET_DB_PASSWORD,
                charset='utf8mb4',
                autocommit=False
            )
            logger.info(f"Connected to target database: {TARGET_DB_HOST}:{TARGET_DB_PORT}")
            return True
        except Exception as e:
            logger.error(f"Failed to connect to database: {e}")
            return False

    def map_database(self, source_db: str) -> str:
        """映射数据库名"""
        return DB_MAPPING.get(source_db, source_db)

    def build_insert_sql(self, table_name: str, columns: List[str]) -> str:
        """构建INSERT SQL"""
        placeholders = ', '.join(['%s'] * len(columns))
        columns_str = ', '.join(columns)
        return f"INSERT INTO {table_name} ({columns_str}) VALUES ({placeholders})"

    def build_update_sql(self, table_name: str, columns: List[str], pk_columns: List[str]) -> str:
        """构建UPDATE SQL"""
        set_clause = ', '.join([f"{col} = %s" for col in columns])
        where_clause = ' AND '.join([f"{pk} = %s" for pk in pk_columns])
        return f"UPDATE {table_name} SET {set_clause} WHERE {where_clause}"

    def build_delete_sql(self, table_name: str, pk_columns: List[str]) -> str:
        """构建DELETE SQL"""
        where_clause = ' AND '.join([f"{pk} = %s" for pk in pk_columns])
        return f"DELETE FROM {table_name} WHERE {where_clause}"

    def process_insert(self, database: str, table: str, columns: Dict[str, any]):
        """处理INSERT事件"""
        try:
            target_db = self.map_database(database)
            cursor = self.db_conn.cursor()

            column_names = list(columns.keys())
            column_values = [columns[col] for col in column_names]

            sql = self.build_insert_sql(f"{target_db}.{table}", column_names)
            cursor.execute(sql, column_values)

            self.stats['insert_count'] += 1
            logger.debug(f"INSERT: {target_db}.{table}")

        except Exception as e:
            logger.error(f"INSERT failed: {e}")
            self.stats['error_count'] += 1
            self.db_conn.rollback()

    def process_update(self, database: str, table: str, before_columns: Dict, after_columns: Dict, pk_columns: List[str]):
        """处理UPDATE事件"""
        try:
            target_db = self.map_database(database)
            cursor = self.db_conn.cursor()

            # 使用after_columns构建SET子句
            column_names = list(after_columns.keys())
            column_values = [after_columns[col] for col in column_names]

            # 使用before_columns构建WHERE子句（主键值）
            pk_values = [before_columns[pk] for pk in pk_columns]

            sql = self.build_update_sql(f"{target_db}.{table}", column_names, pk_columns)
            cursor.execute(sql, column_values + pk_values)

            self.stats['update_count'] += 1
            logger.debug(f"UPDATE: {target_db}.{table}")

        except Exception as e:
            logger.error(f"UPDATE failed: {e}")
            self.stats['error_count'] += 1
            self.db_conn.rollback()

    def process_delete(self, database: str, table: str, columns: Dict, pk_columns: List[str]):
        """处理DELETE事件"""
        try:
            target_db = self.map_database(database)
            cursor = self.db_conn.cursor()

            # 使用主键值构建WHERE子句
            pk_values = [columns[pk] for pk in pk_columns]

            sql = self.build_delete_sql(f"{target_db}.{table}", pk_columns)
            cursor.execute(sql, pk_values)

            self.stats['delete_count'] += 1
            logger.debug(f"DELETE: {target_db}.{table}")

        except Exception as e:
            logger.error(f"DELETE failed: {e}")
            self.stats['error_count'] += 1
            self.db_conn.rollback()

    def process_entry(self, entry):
        """处理单个Entry"""
        try:
            # 解析Entry
            entry_type = entry.entryType
            if entry_type != EntryProtocol_pb2.ROWDATA:
                return

            # 解析RowChange
            row_change = EntryProtocol_pb2.RowChange()
            row_change.ParseFromString(entry.storeValue)

            event_type = row_change.eventType
            database = entry.header.schemaName
            table = entry.header.tableName

            # 处理每一行数据
            for row in row_change.rowDatas:
                if event_type == EntryProtocol_pb2.EventType.INSERT:
                    # INSERT事件
                    columns = {col.name: col.value for col in row.afterColumns}
                    self.process_insert(database, table, columns)

                elif event_type == EntryProtocol_pb2.EventType.UPDATE:
                    # UPDATE事件
                    before_columns = {col.name: col.value for col in row.beforeColumns}
                    after_columns = {col.name: col.value for col in row.afterColumns}
                    pk_columns = [col.name for col in row.afterColumns if col.isKey]
                    self.process_update(database, table, before_columns, after_columns, pk_columns)

                elif event_type == EntryProtocol_pb2.EventType.DELETE:
                    # DELETE事件
                    columns = {col.name: col.value for col in row.beforeColumns}
                    pk_columns = [col.name for col in row.beforeColumns if col.isKey]
                    self.process_delete(database, table, columns, pk_columns)

                self.stats['total_events'] += 1

        except Exception as e:
            logger.error(f"Process entry failed: {e}")
            self.stats['error_count'] += 1

    def run(self):
        """主循环"""
        if not self.connect_canal():
            return False

        if not self.connect_db():
            return False

        self.running = True
        logger.info("Canal sync client started")

        try:
            batch_count = 0
            last_commit_time = time.time()

            while self.running:
                # 获取binlog数据
                message = self.canal_client.get(BATCH_SIZE)

                if message.id == -1 or len(message.entries) == 0:
                    # 没有数据，等待1秒
                    time.sleep(1)
                    continue

                # 处理Entry
                for entry in message.entries:
                    self.process_entry(entry)
                    batch_count += 1

                # 批量提交
                current_time = time.time()
                if batch_count >= BATCH_SIZE or (current_time - last_commit_time) >= BATCH_TIMEOUT:
                    self.db_conn.commit()
                    self.canal_client.ack(message.id)  # 确认消息

                    # 更新统计
                    self.stats['last_sync_time'] = datetime.now()
                    logger.info(f"Committed batch: {batch_count} events, "
                               f"Total: {self.stats['total_events']}, "
                               f"Errors: {self.stats['error_count']}")

                    batch_count = 0
                    last_commit_time = current_time

        except KeyboardInterrupt:
            logger.info("Received stop signal")
        except Exception as e:
            logger.error(f"Sync error: {e}")
        finally:
            self.stop()

    def stop(self):
        """停止客户端"""
        self.running = False

        if self.db_conn:
            try:
                self.db_conn.commit()
                self.db_conn.close()
                logger.info("Database connection closed")
            except:
                pass

        if self.canal_client:
            try:
                self.canal_client.disconnect()
                logger.info("Canal connection closed")
            except:
                pass

        logger.info(f"Final stats: {json.dumps(self.stats, default=str)}")

# ========================================
# 主函数
# ========================================

if __name__ == '__main__':
    client = CanalSyncClient()
    client.run()
