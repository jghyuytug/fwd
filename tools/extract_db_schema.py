#!/usr/bin/env python3
"""
数据库Schema提取工具
从df_game_r.c反编译代码中提取数据库结构

功能：
1. 提取SQL查询语句
2. 分析表结构和字段
3. 推导表关系
4. 生成初始schema.sql

作者：自动分析工具
日期：2025-10-21
"""

import re
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple
from collections import defaultdict

class DBSchemaExtractor:
    def __init__(self, source_file: str):
        self.source_file = Path(source_file)
        self.tables = defaultdict(set)  # table_name -> set of columns
        self.queries = {
            'SELECT': [],
            'INSERT': [],
            'UPDATE': [],
            'DELETE': [],
            'CREATE': []
        }

    def extract(self):
        """主提取流程"""
        print(f"[*] 分析文件: {self.source_file}")

        with open(self.source_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        print(f"[+] 文件大小: {len(content)} 字节")

        # 1. 提取SQL查询
        self._extract_sql_queries(content)

        # 2. 分析表结构
        self._analyze_table_structure()

        # 3. 查找Taiwan特定表
        self._find_taiwan_tables(content)

        # 4. 生成SQL Schema
        return self._generate_schema()

    def _extract_sql_queries(self, content: str):
        """提取SQL查询语句"""
        print("\n[*] 提取SQL查询...")

        # SQL查询模式（在字符串中）
        sql_pattern = r'"((?:SELECT|INSERT|UPDATE|DELETE|CREATE)\s+.*?)"'

        matches = re.finditer(sql_pattern, content, re.IGNORECASE | re.DOTALL)

        for match in matches:
            query = match.group(1)
            # 清理换行和多余空格
            query = re.sub(r'\s+', ' ', query).strip()

            # 分类查询
            if query.upper().startswith('SELECT'):
                self.queries['SELECT'].append(query)
            elif query.upper().startswith('INSERT'):
                self.queries['INSERT'].append(query)
            elif query.upper().startswith('UPDATE'):
                self.queries['UPDATE'].append(query)
            elif query.upper().startswith('DELETE'):
                self.queries['DELETE'].append(query)
            elif query.upper().startswith('CREATE'):
                self.queries['CREATE'].append(query)

        print(f"[+] SELECT查询: {len(self.queries['SELECT'])} 个")
        print(f"[+] INSERT查询: {len(self.queries['INSERT'])} 个")
        print(f"[+] UPDATE查询: {len(self.queries['UPDATE'])} 个")
        print(f"[+] DELETE查询: {len(self.queries['DELETE'])} 个")
        print(f"[+] CREATE查询: {len(self.queries['CREATE'])} 个")

        # 显示示例
        if self.queries['SELECT']:
            print("\n[示例] SELECT查询:")
            for query in self.queries['SELECT'][:3]:
                print(f"    {query[:100]}...")

        if self.queries['INSERT']:
            print("\n[示例] INSERT查询:")
            for query in self.queries['INSERT'][:3]:
                print(f"    {query[:100]}...")

    def _analyze_table_structure(self):
        """分析表结构"""
        print("\n[*] 分析表结构...")

        # 从SELECT提取表名和列名
        for query in self.queries['SELECT']:
            self._parse_select_query(query)

        # 从INSERT提取表名和列名
        for query in self.queries['INSERT']:
            self._parse_insert_query(query)

        # 从UPDATE提取表名和列名
        for query in self.queries['UPDATE']:
            self._parse_update_query(query)

        print(f"[+] 发现 {len(self.tables)} 个表:")
        for table, columns in sorted(self.tables.items())[:20]:
            print(f"    - {table}: {len(columns)} 列")
            if columns:
                sample_cols = list(columns)[:5]
                print(f"      样例: {', '.join(sample_cols)}")

        if len(self.tables) > 20:
            print(f"    ... 还有 {len(self.tables)-20} 个表")

    def _parse_select_query(self, query: str):
        """解析SELECT查询"""
        # SELECT col1, col2 FROM table_name WHERE ...
        match = re.search(r'SELECT\s+(.*?)\s+FROM\s+(\w+)', query, re.IGNORECASE)
        if match:
            columns_str = match.group(1)
            table_name = match.group(2)

            # 提取列名
            columns = re.findall(r'(\w+)(?:\s+AS\s+\w+)?', columns_str)
            columns = [c for c in columns if c.upper() not in ('SELECT', 'DISTINCT')]

            if table_name not in ('WHERE', 'JOIN', 'LEFT', 'RIGHT', 'INNER'):
                self.tables[table_name].update(columns)

    def _parse_insert_query(self, query: str):
        """解析INSERT查询"""
        # INSERT INTO table_name (col1, col2) VALUES (?, ?)
        match = re.search(r'INSERT\s+INTO\s+(\w+)\s*\((.*?)\)', query, re.IGNORECASE)
        if match:
            table_name = match.group(1)
            columns_str = match.group(2)

            columns = [c.strip() for c in columns_str.split(',')]
            self.tables[table_name].update(columns)

    def _parse_update_query(self, query: str):
        """解析UPDATE查询"""
        # UPDATE table_name SET col1=?, col2=? WHERE ...
        match = re.search(r'UPDATE\s+(\w+)\s+SET\s+(.*?)(?:\s+WHERE|$)', query, re.IGNORECASE)
        if match:
            table_name = match.group(1)
            set_clause = match.group(2)

            # 提取列名
            columns = re.findall(r'(\w+)\s*=', set_clause)
            self.tables[table_name].update(columns)

    def _find_taiwan_tables(self, content: str):
        """查找Taiwan特定的数据库代码"""
        print("\n[*] 查找Taiwan特定表...")

        # 查找Taiwan命名空间的Query类
        taiwan_patterns = [
            r'Taiwan::(\w+Query)',
            r'Taiwan::(\w+Table)',
        ]

        taiwan_classes = set()
        for pattern in taiwan_patterns:
            matches = re.findall(pattern, content)
            taiwan_classes.update(matches)

        print(f"[+] 发现 {len(taiwan_classes)} 个Taiwan特定类:")
        for cls in sorted(taiwan_classes):
            print(f"    - Taiwan::{cls}")

            # 推测对应的表名
            table_name = re.sub(r'Query$', '', cls)
            table_name = re.sub(r'([A-Z])', r'_\1', table_name).lower().strip('_')
            print(f"      可能的表名: {table_name}")

    def _generate_schema(self) -> str:
        """生成SQL Schema"""
        print("\n[*] 生成数据库Schema...")

        schema = """-- ============================================================
-- df_game_r 数据库Schema
-- 自动生成于: 2025-10-21
-- 来源: 从反编译代码推导
--
-- 注意: 这是推测的结构，需要根据实际情况调整
-- ============================================================

"""

        # 分4个数据库生成
        databases = {
            'ARAD_DB_ACCOUNT': ['account', 'login', 'session', 'ban'],
            'ARAD_DB_CHARACTER': ['character', 'charac', 'inventory', 'skill', 'quest', 'avatar'],
            'ARAD_DB_GAME': ['guild', 'party', 'trade', 'mail', 'auction', 'taiwan'],
            'ARAD_DB_LOG': ['log', 'history', 'event']
        }

        for db_name, keywords in databases.items():
            schema += f"\n-- ============================================================\n"
            schema += f"-- 数据库: {db_name}\n"
            schema += f"-- ============================================================\n\n"
            schema += f"CREATE DATABASE IF NOT EXISTS {db_name} CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;\n"
            schema += f"USE {db_name};\n\n"

            # 找到相关表
            related_tables = []
            for table in self.tables.keys():
                if any(kw in table.lower() for kw in keywords):
                    related_tables.append(table)

            if not related_tables:
                schema += f"-- (未从代码中提取到相关表，使用推测结构)\n\n"
                schema += self._generate_default_schema_for_db(db_name)
            else:
                for table in sorted(related_tables):
                    schema += self._generate_table_ddl(table, self.tables[table])

        return schema

    def _generate_table_ddl(self, table_name: str, columns: Set[str]) -> str:
        """生成表DDL"""
        ddl = f"-- 表: {table_name}\n"
        ddl += f"CREATE TABLE IF NOT EXISTS {table_name} (\n"

        if not columns:
            ddl += "    -- (未提取到列信息，需手工补充)\n"
            ddl += "    id INT PRIMARY KEY AUTO_INCREMENT\n"
        else:
            # 推测主键
            pk_candidates = [c for c in columns if 'id' in c.lower() or 'no' in c.lower()]
            primary_key = pk_candidates[0] if pk_candidates else 'id'

            if primary_key not in columns:
                ddl += f"    {primary_key} INT PRIMARY KEY AUTO_INCREMENT,\n"

            for col in sorted(columns):
                col_type = self._guess_column_type(col)
                is_pk = (col == primary_key)

                ddl += f"    {col} {col_type}"
                if is_pk:
                    ddl += " PRIMARY KEY"
                ddl += ",\n"

            # 移除最后的逗号
            ddl = ddl.rstrip(',\n') + "\n"

        ddl += ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='从代码提取';\n\n"
        return ddl

    def _guess_column_type(self, column_name: str) -> str:
        """推测列类型"""
        col_lower = column_name.lower()

        # ID类型
        if col_lower.endswith('_id') or col_lower.endswith('_no') or col_lower == 'id':
            return 'INT'

        # 名称类型
        if 'name' in col_lower or col_lower.endswith('_nm'):
            return 'VARCHAR(64)'

        # 时间类型
        if 'time' in col_lower or 'date' in col_lower or col_lower.startswith('dt_'):
            return 'DATETIME'

        # 计数器
        if 'count' in col_lower or 'cnt' in col_lower or 'num' in col_lower:
            return 'INT'

        # 金额/经验值
        if 'gold' in col_lower or 'exp' in col_lower or 'money' in col_lower or 'cash' in col_lower:
            return 'BIGINT'

        # 等级
        if 'level' in col_lower or 'lv' in col_lower:
            return 'SMALLINT'

        # 标志位
        if 'flag' in col_lower or 'is_' in col_lower or col_lower.startswith('b_'):
            return 'TINYINT'

        # IP地址
        if 'ip' in col_lower:
            return 'VARCHAR(15)'

        # 默认
        return 'VARCHAR(255)'

    def _generate_default_schema_for_db(self, db_name: str) -> str:
        """为数据库生成默认Schema"""

        if db_name == 'ARAD_DB_ACCOUNT':
            return """-- 账号表
CREATE TABLE IF NOT EXISTS accounts (
    m_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '账号ID',
    account_id VARCHAR(64) UNIQUE NOT NULL COMMENT '登录账号',
    password_hash VARCHAR(128) COMMENT '密码哈希',
    email VARCHAR(128) COMMENT '邮箱',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    last_login DATETIME COMMENT '最后登录时间',
    login_ip VARCHAR(15) COMMENT '登录IP',
    status TINYINT DEFAULT 0 COMMENT '状态 0=正常 1=封禁'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='账号表';

-- 会话表
CREATE TABLE IF NOT EXISTS account_sessions (
    session_id BIGINT PRIMARY KEY COMMENT '会话ID',
    account_id INT NOT NULL COMMENT '账号ID',
    server_group INT COMMENT '服务器组',
    login_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '登录时间',
    ip_address VARCHAR(15) COMMENT 'IP地址',
    FOREIGN KEY (account_id) REFERENCES accounts(m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='会话表';

"""

        elif db_name == 'ARAD_DB_CHARACTER':
            return """-- 角色表
CREATE TABLE IF NOT EXISTS characters (
    charac_no INT PRIMARY KEY AUTO_INCREMENT COMMENT '角色编号',
    m_id INT NOT NULL COMMENT '账号ID',
    charac_name VARCHAR(32) UNIQUE NOT NULL COMMENT '角色名',
    job INT COMMENT '职业',
    level SMALLINT DEFAULT 1 COMMENT '等级',
    exp BIGINT DEFAULT 0 COMMENT '经验值',
    hp INT COMMENT '当前HP',
    mp INT COMMENT '当前MP',
    gold BIGINT DEFAULT 0 COMMENT '金币',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    delete_flag TINYINT DEFAULT 0 COMMENT '删除标记',
    INDEX idx_account (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='角色表';

-- 背包表
CREATE TABLE IF NOT EXISTS character_inventory (
    item_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '物品ID',
    charac_no INT NOT NULL COMMENT '角色编号',
    slot_no INT COMMENT '槽位',
    item_code INT COMMENT '物品代码',
    quantity INT DEFAULT 1 COMMENT '数量',
    upgrade_level TINYINT DEFAULT 0 COMMENT '强化等级',
    enchant_data BLOB COMMENT '附魔数据',
    FOREIGN KEY (charac_no) REFERENCES characters(charac_no),
    INDEX idx_character (charac_no)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='背包表';

-- 任务表
CREATE TABLE IF NOT EXISTS character_quests (
    charac_no INT COMMENT '角色编号',
    quest_id INT COMMENT '任务ID',
    state TINYINT DEFAULT 0 COMMENT '状态 0=未接取 1=进行中 2=已完成',
    progress BLOB COMMENT '进度数据',
    accept_time DATETIME COMMENT '接取时间',
    complete_time DATETIME COMMENT '完成时间',
    PRIMARY KEY (charac_no, quest_id),
    FOREIGN KEY (charac_no) REFERENCES characters(charac_no)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='任务表';

"""

        elif db_name == 'ARAD_DB_GAME':
            return """-- 公会表
CREATE TABLE IF NOT EXISTS guild_info (
    guild_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '公会ID',
    guild_name VARCHAR(64) UNIQUE NOT NULL COMMENT '公会名',
    master_charac_no INT COMMENT '会长角色编号',
    level INT DEFAULT 1 COMMENT '公会等级',
    exp BIGINT DEFAULT 0 COMMENT '公会经验',
    notice TEXT COMMENT '公会公告',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='公会表';

-- 组队表
CREATE TABLE IF NOT EXISTS party_data (
    party_id BIGINT PRIMARY KEY COMMENT '队伍ID',
    leader_charac_no INT COMMENT '队长角色编号',
    dungeon_id INT COMMENT '副本ID',
    clear_state INT DEFAULT 0 COMMENT '通关状态',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='组队表';

-- Taiwan现金系统 - 余额表
CREATE TABLE IF NOT EXISTS taiwan_cash_balance (
    m_id INT PRIMARY KEY COMMENT '账号ID',
    balance BIGINT DEFAULT 0 COMMENT '余额',
    currency_type INT COMMENT '货币类型',
    last_update DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后更新'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Taiwan现金余额';

-- Taiwan现金系统 - 购买记录
CREATE TABLE IF NOT EXISTS taiwan_cash_purchase (
    purchase_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '购买ID',
    m_id INT NOT NULL COMMENT '账号ID',
    item_code INT COMMENT '物品代码',
    quantity INT COMMENT '数量',
    price BIGINT COMMENT '价格',
    purchase_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '购买时间',
    INDEX idx_account (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Taiwan购买记录';

"""

        elif db_name == 'ARAD_DB_LOG':
            return """-- 登录日志
CREATE TABLE IF NOT EXISTS login_log (
    log_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '日志ID',
    m_id INT COMMENT '账号ID',
    login_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '登录时间',
    logout_time DATETIME COMMENT '登出时间',
    ip_address VARCHAR(15) COMMENT 'IP地址',
    result TINYINT COMMENT '结果 0=成功 1=失败',
    INDEX idx_account (m_id),
    INDEX idx_time (login_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='登录日志';

-- 物品日志
CREATE TABLE IF NOT EXISTS item_log (
    log_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '日志ID',
    charac_no INT COMMENT '角色编号',
    action_type TINYINT COMMENT '动作 1=获得 2=丢弃 3=交易',
    item_code INT COMMENT '物品代码',
    quantity INT COMMENT '数量',
    log_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '记录时间',
    INDEX idx_character (charac_no),
    INDEX idx_time (log_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='物品日志';

-- 游戏事件日志
CREATE TABLE IF NOT EXISTS event_log (
    log_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '日志ID',
    event_type VARCHAR(32) COMMENT '事件类型',
    charac_no INT COMMENT '角色编号',
    event_data TEXT COMMENT '事件数据',
    log_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '记录时间',
    INDEX idx_type (event_type),
    INDEX idx_time (log_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='事件日志';

"""

        return ""

def main():
    if len(sys.argv) < 2:
        print(f"用法: {sys.argv[0]} <df_game_r.c>")
        print(f"示例: {sys.argv[0]} df_game_r.c > schema/database_schema.sql")
        sys.exit(1)

    source_file = sys.argv[1]

    if not Path(source_file).exists():
        print(f"[!] 错误: 文件不存在 {source_file}", file=sys.stderr)
        sys.exit(1)

    extractor = DBSchemaExtractor(source_file)
    schema_sql = extractor.extract()

    print("\n" + "="*60)
    print("生成的数据库Schema:")
    print("="*60)
    print(schema_sql)

    # 保存到文件
    output_file = Path("schema/database_schema.sql")
    output_file.parent.mkdir(exist_ok=True)
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(schema_sql)

    print(f"\n[+] Schema已保存到: {output_file}", file=sys.stderr)
    print(f"[+] 请导入到MySQL进行测试和调整", file=sys.stderr)

    # 生成导入脚本
    import_script = Path("schema/import_schema.sh")
    with open(import_script, 'w', encoding='utf-8') as f:
        f.write("""#!/bin/bash
# 导入数据库Schema

MYSQL_USER="arad_user"
MYSQL_PASS="your_password_here"

echo "[*] 导入数据库Schema..."
mysql -u $MYSQL_USER -p$MYSQL_PASS < database_schema.sql

echo "[+] 导入完成"
echo "[*] 验证数据库..."
mysql -u $MYSQL_USER -p$MYSQL_PASS -e "SHOW DATABASES LIKE 'ARAD_DB_%';"
""")
    import_script.chmod(0o755)

    print(f"[+] 导入脚本已保存到: {import_script}", file=sys.stderr)

if __name__ == '__main__':
    main()
