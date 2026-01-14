#include "db_connection_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <limits.h>

// 加载数据库配置文件
static int LoadDatabaseConfig(DBConnectionManager* manager, const char* config_file) {
    FILE* fp = fopen(config_file, "r");
    if (!fp) {
        fprintf(stderr, "[Database] ERROR: Failed to open config file %s\n", config_file);
        return -1;
    }

    char line[512];
    int db_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL && db_count < 16) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        DatabaseConfig* config = &manager->configs[db_count + 1];  // 从索引1开始

        // 解析配置行: type,host,port,user,password,database,pool_size,timeout
        int parsed = sscanf(line, "%d,%63[^,],%d,%31[^,],%127[^,],%63[^,],%d,%d",
                           &config->type, config->host, &config->port, config->user,
                           config->password, config->database, &config->pool_size, &config->timeout);

        if (parsed != 8) {
            continue;  // 跳过无效行
        }

        printf("[Database] Loaded config for %s (type=%d, pool_size=%d)\n",
               config->database, config->type, config->pool_size);

        db_count++;
    }

    fclose(fp);
    return 0;
}

// 初始化数据库管理器
int DBConnectionManager_Initialize(DBConnectionManager* self, const char* config_file) {
    // 加载配置文件
    if (LoadDatabaseConfig(self, config_file) != 0) {
        return -1;
    }

    // 初始化连接池
    for (int i = 0; i <= DB_TYPE_MAX_INDEX; i++) {
        self->pools[i].connections = NULL;
        self->pools[i].pool_size = 0;
        self->pools[i].available = 0;
        self->pools[i].max_size = (i > 0) ? self->configs[i].pool_size : 0;
        self->pools[i].in_use = 0;
    }

    // 初始化统计
    self->total_queries = 0;
    self->failed_queries = 0;
    self->connection_errors = 0;
    self->initialized = true;

    return 0;
}

// 从连接池获取连接
static DatabaseConnection* GetConnectionFromPool(DBConnectionManager* self, int db_type) {
    ConnectionPool* pool = &self->pools[db_type];
    
    printf("[POOL_DEBUG] GetConnectionFromPool: db_type=%d\n", db_type);
    printf("[POOL_DEBUG] pool_size=%d, available=%d, in_use=%d\n", pool->pool_size, pool->available, pool->in_use);

    // 查找可用连接
    for (int i = 0; i < pool->pool_size; i++) {
        printf("[POOL_DEBUG] Connection[%d]: is_connected=%d, in_use=%d, mysql_conn=%p\n", 
               i, pool->connections[i].is_connected, pool->connections[i].in_use, 
               pool->connections[i].mysql_conn);
               
        if (pool->connections[i].is_connected && !pool->connections[i].mysql_conn) {
            printf("[POOL_DEBUG] Connection[%d]: Skipping (mysql_conn is NULL)\n", i);
            continue;
        }
        if (pool->connections[i].is_connected && !pool->connections[i].in_use) {
            printf("[POOL_DEBUG] Connection[%d]: Acquired!\n", i);
            pool->connections[i].in_use = true;
            pool->available--;
            pool->in_use++;
            return &pool->connections[i];
        }
    }

    printf("[POOL_DEBUG] No available connection found!\n");
    return NULL;
}

// 将连接归还到连接池
static void ReleaseConnectionToPool(DBConnectionManager* self, DatabaseConnection* conn) {
    ConnectionPool* pool = &self->pools[conn->type];
    pool->available++;
    pool->in_use--;
    conn->last_used = time(NULL);
    conn->in_use = false;
}

// 连接到指定类型的数据库
int DBConnectionManager_Connect(DBConnectionManager* self, int db_type) {
    if (db_type < 1 || db_type > DB_TYPE_MAX_INDEX) {
        fprintf(stderr, "[Database] ERROR: Invalid database type %d\n", db_type);
        return -1;
    }

    ConnectionPool* pool = &self->pools[db_type];
    DatabaseConfig* config = &self->configs[db_type];

    // 分配连接数组
    if (pool->connections == NULL) {
        pool->connections = malloc(config->pool_size * sizeof(DatabaseConnection));
        if (!pool->connections) {
            fprintf(stderr, "[Database] ERROR: Failed to allocate connection pool\n");
            return -1;
        }
        memset(pool->connections, 0, config->pool_size * sizeof(DatabaseConnection));
    }

    // 创建连接
    for (int i = 0; i < config->pool_size; i++) {
        DatabaseConnection* conn = &pool->connections[i];
        conn->type = db_type;
        conn->last_used = time(NULL);
        conn->use_count = 0;

        MYSQL* mysql_conn = mysql_init(NULL);
        if (!mysql_conn) {
            fprintf(stderr, "[Database] ERROR: Failed to initialize MySQL\n");
            return -1;
        }

        if (!mysql_real_connect(mysql_conn, config->host, config->user, config->password,
                                config->database, config->port, NULL, 0)) {
            fprintf(stderr, "[Database] ERROR: Failed to connect to %s: %s\n",
                    config->database, mysql_error(mysql_conn));
            mysql_close(mysql_conn);
            return -1;
        }

        // 设置字符集为UTF8
        if (mysql_query(mysql_conn, "SET NAMES utf8") != 0) {
            fprintf(stderr, "[Database] WARNING: Failed to set charset to utf8: %s\n",
                    mysql_error(mysql_conn));
        }

        conn->mysql_conn = (MYSQL_CONNECTION*)mysql_conn;
        conn->in_use = false;
        conn->is_connected = true;
        pool->pool_size++;
        pool->available++;
    }

    printf("[Database] Connected to %s with %d connections\n", config->database, pool->pool_size);
    return 0;
}

// 连接到所有数据库
int DBConnectionManager_ConnectAll(DBConnectionManager* self) {
    for (int i = 1; i <= DB_TYPE_MAX_INDEX; i++) {
        if (DBConnectionManager_Connect(self, i) != 0) {
            fprintf(stderr, "[Database] ERROR: Failed to connect to database type %d\n", i);
            return -1;
        }
    }
    return 0;
}

// 执行查询
int DBConnectionManager_ExecuteQuery(DBConnectionManager* self, int db_type, const char* query, DBQueryResult* result) {
    DatabaseConnection* conn = GetConnectionFromPool(self, db_type);
    if (!conn) {
        fprintf(stderr, "[Database] ERROR: No available connection\n");
        return -1;
    }

    MYSQL* mysql_conn = (MYSQL*)conn->mysql_conn;
    if (mysql_query(mysql_conn, query) != 0) {
        fprintf(stderr, "[Database] ERROR: Query failed: %s\n", mysql_error(mysql_conn));
        self->failed_queries++;
        ReleaseConnectionToPool(self, conn);
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(mysql_conn);
    if (res) {
        result->result_set = (MYSQL_RESULT_SET*)res;
        result->row_count = mysql_num_rows(res);
        result->column_count = mysql_num_fields(res);
        result->current_row = 0;
    } else {
        // INSERT/UPDATE/DELETE等不返回结果集的查询
        result->result_set = NULL;
        result->row_count = mysql_affected_rows(mysql_conn);
        result->column_count = 0;
        result->current_row = 0;
    }

    conn->use_count++;
    self->total_queries++;
    ReleaseConnectionToPool(self, conn);
    return result->row_count;
}

int DBConnectionManager_ExecuteInsert(DBConnectionManager* self, int db_type, const char* query, unsigned long long* insert_id) {
    DatabaseConnection* conn = GetConnectionFromPool(self, db_type);
    if (!conn) {
        fprintf(stderr, "[Database] ERROR: No available connection\n");
        return -1;
    }

    MYSQL* mysql_conn = (MYSQL*)conn->mysql_conn;
    if (mysql_query(mysql_conn, query) != 0) {
        fprintf(stderr, "[Database] ERROR: Query failed: %s\n", mysql_error(mysql_conn));
        self->failed_queries++;
        ReleaseConnectionToPool(self, conn);
        return -1;
    }

    if (insert_id) {
        *insert_id = mysql_insert_id(mysql_conn);
    }

    MYSQL_RES* res = mysql_store_result(mysql_conn);
    if (res) {
        mysql_free_result(res);
    }

    conn->use_count++;
    self->total_queries++;
    ReleaseConnectionToPool(self, conn);
    return 0;
}

// 获取下一行
int DBQueryResult_FetchRow(DBQueryResult* self, char** values) {
    if (!self->result_set) {
        return 0;
    }

    MYSQL_RES* res = (MYSQL_RES*)self->result_set;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        return 0;
    }

    for (int i = 0; i < self->column_count; i++) {
        values[i] = row[i];
    }

    self->current_row++;
    return self->column_count;
}

// 获取下一行（带字段长度信息）- 用于BLOB字段
int DBQueryResult_FetchRowWithLengths(DBQueryResult* self, char** values, unsigned long** lengths) {
    if (!self->result_set) {
        return 0;
    }

    MYSQL_RES* res = (MYSQL_RES*)self->result_set;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        return 0;
    }

    // 获取字段长度数组
    unsigned long* field_lengths = mysql_fetch_lengths(res);
    if (!field_lengths) {
        return 0;
    }

    // 填充值和长度
    for (int i = 0; i < self->column_count; i++) {
        values[i] = row[i];
    }
    *lengths = field_lengths;

    self->current_row++;
    return self->column_count;
}

// 释放结果集
void DBQueryResult_Free(DBQueryResult* self) {
    if (self->result_set) {
        mysql_free_result((MYSQL_RES*)self->result_set);
        self->result_set = NULL;
    }
    self->row_count = 0;
    self->column_count = 0;
    self->current_row = 0;
}

// 清理并关闭所有连接
void DBConnectionManager_Cleanup(DBConnectionManager* self) {
    if (!self->initialized) {
        return;
    }

    for (int i = 0; i <= DB_TYPE_MAX_INDEX; i++) {
        ConnectionPool* pool = &self->pools[i];
        if (pool->connections) {
            for (int j = 0; j < pool->pool_size; j++) {
                if (pool->connections[j].mysql_conn) {
                    mysql_close((MYSQL*)pool->connections[j].mysql_conn);
                }
            }
            free(pool->connections);
            pool->connections = NULL;
            pool->pool_size = 0;
        }
    }

    self->initialized = false;
    printf("[Database] All connections closed successfully\n");
}

// 检查是否已连接
bool DBConnectionManager_IsConnected(DBConnectionManager* self, int db_type) {
    if (db_type < 1 || db_type > DB_TYPE_MAX_INDEX) {
        return false;
    }
    return self->pools[db_type].pool_size > 0;
}

// 获取统计信息
void DBConnectionManager_GetStats(DBConnectionManager* self, int db_type __attribute__((unused)),
                                  int* total_queries, int* failed_queries) {
    if (total_queries) {
        *total_queries = self->total_queries;
    }
    if (failed_queries) {
        *failed_queries = self->failed_queries;
    }
}

// 开始事务
int DBConnectionManager_BeginTransaction(DBConnectionManager* self, int db_type) {
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    return DBConnectionManager_ExecuteQuery(self, db_type, "START TRANSACTION", &result);
}

// 提交事务
int DBConnectionManager_CommitTransaction(DBConnectionManager* self, int db_type) {
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    return DBConnectionManager_ExecuteQuery(self, db_type, "COMMIT", &result);
}

// 回滚事务
int DBConnectionManager_RollbackTransaction(DBConnectionManager* self, int db_type) {
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    return DBConnectionManager_ExecuteQuery(self, db_type, "ROLLBACK", &result);
}

// 获取连接池统计信息
int DBConnectionManager_GetConnectionPoolStats(DBConnectionManager* self, int db_type,
                                               int* total, int* available, int* inuse) {
    if (db_type < 1 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    ConnectionPool* pool = &self->pools[db_type];
    if (total) *total = pool->pool_size;
    if (available) *available = pool->available;
    if (inuse) *inuse = pool->in_use;

    return 0;
}
