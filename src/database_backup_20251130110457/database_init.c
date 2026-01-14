#include "database_interface.h"
#include "db_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>
#include <limits.h>

// Load database configuration from file
static int LoadDatabaseConfig(CDBConnector* connector, const char* config_file) {
    FILE* fp = fopen(config_file, "r");
    if (!fp) {
        fprintf(stderr, "[Database] ERROR: Failed to open config file %s\n", config_file);
        return -1;
    }

    char line[512];
    int db_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL && db_count <= DB_TYPE_MAX_INDEX) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        DBConfig* config = &connector->configs[db_count];

        // 解析配置行: type,host,port,user,password,database,pool_size,timeout
        int parsed = sscanf(line, "%d,%63[^,],%d,%31[^,],%127[^,],%63[^,],%d,%d",
                           &config->type, config->host, &config->port, config->user,
                           config->password, config->database, &config->pool_size, &config->timeout);

        if (parsed != 8) {
            fprintf(stderr, "[Database] WARNING: Skipping invalid line: %s", line);
            continue;
        }

        // 验证配置
        if (config->type < 1 || config->type > DB_TYPE_MAX_INDEX) {
            fprintf(stderr, "[Database] ERROR: Invalid database type %d\n", config->type);
            fclose(fp);
            return -1;
        }

        if (config->port < 1 || config->port > 65535) {
            fprintf(stderr, "[Database] ERROR: Invalid port %d for database type %d\n",
                    config->port, config->type);
            fclose(fp);
            return -1;
        }

        if (config->pool_size < 1 || config->pool_size > 1000) {
            fprintf(stderr, "[Database] ERROR: Invalid pool size %d for database type %d\n",
                    config->pool_size, config->type);
            fclose(fp);
            return -1;
        }

        if (config->timeout < 1 || config->timeout > 600) {
            fprintf(stderr, "[Database] ERROR: Invalid timeout %d for database type %d\n",
                    config->timeout, config->type);
            fclose(fp);
            return -1;
        }

        printf("[Database] Loaded config for %s (type=%d, pool_size=%d)\n",
               config->database, config->type, config->pool_size);

        db_count++;
    }

    fclose(fp);

    if (db_count != DB_TYPE_MAX_INDEX) {
        fprintf(stderr, "[Database] WARNING: Expected %d databases, loaded %d\n",
                DB_TYPE_MAX_INDEX, db_count);
    }

    return 0;
}

// Initialize database connector with configuration file
int CDBConnector_Initialize(CDBConnector* self, const char* config_file) {
    // Load configuration from file
    if (LoadDatabaseConfig(self, config_file) != 0) {
        return -1;
    }

    // Initialize connection pools
    for (int i = 0; i <= DB_TYPE_MAX_INDEX; i++) {
        self->pools[i].connections = NULL;
        self->pools[i].pool_size = 0;
        self->pools[i].available = 0;
        self->pools[i].max_size = self->configs[i].pool_size;
        self->pools[i].in_use = 0;
    }

    // Initialize statistics
    self->total_queries = 0;
    self->failed_queries = 0;
    self->connection_errors = 0;

    self->initialized = true;
    return 0;
}

// Get connection from pool
static DBConnection* GetConnectionFromPool(CDBConnector* self, int db_type) {
    DBConnectionPool* pool = &self->pools[db_type];

    if (pool->available > 0) {
        // Get available connection from the pool
        for (int i = 0; i < pool->pool_size; i++) {
            if (!pool->connections[i].is_connected) {
                pool->available--;
                pool->in_use++;
                return &pool->connections[i];
            }
        }
    }

    // Create new connection if pool is empty
    DBConnection* conn = malloc(sizeof(DBConnection));
    if (!conn) {
        fprintf(stderr, "[Database] ERROR: Failed to allocate memory for new connection\n");
        return NULL;
    }

    memset(conn, 0, sizeof(DBConnection));
    conn->type = db_type;
    conn->is_connected = false;
    conn->last_used = time(NULL);
    conn->use_count = 0;

    pool->connections = realloc(pool->connections, (pool->pool_size + 1) * sizeof(DBConnection));
    if (!pool->connections) {
        fprintf(stderr, "[Database] ERROR: Failed to allocate memory for connection pool\n");
        free(conn);
        return NULL;
    }

    pool->connections[pool->pool_size] = *conn;
    pool->pool_size++;
    pool->in_use++;
    free(conn);
    return &pool->connections[pool->pool_size - 1];
}

// Release connection back to pool
static void ReleaseConnectionToPool(CDBConnector* self, DBConnection* conn) {
    DBConnectionPool* pool = &self->pools[conn->type];

    if (pool->pool_size > pool->max_size) {
        // Close the oldest connection if pool is full
        int oldest_index = -1;
        int oldest_time = INT_MAX;
        for (int i = 0; i < pool->pool_size; i++) {
            if (pool->connections[i].last_used < oldest_time) {
                oldest_index = i;
                oldest_time = pool->connections[i].last_used;
            }
        }

        if (oldest_index >= 0) {
            mysql_close(pool->connections[oldest_index].mysql_conn);
            pool->pool_size--;
            if (oldest_index != pool->pool_size) {
                pool->connections[oldest_index] = pool->connections[pool->pool_size];
            }
        }
    }

    pool->available++;
    pool->in_use--;
    conn->last_used = time(NULL);
}

// Connect to specific database type
int CDBConnector_Connect(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        fprintf(stderr, "[Database] ERROR: Invalid database type %d\n", db_type);
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    DBConfig* config = &self->configs[db_type];

    // Allocate connection array if not already allocated
    if (pool->connections == NULL) {
        pool->connections = malloc(config->pool_size * sizeof(DBConnection));
        if (!pool->connections) {
            fprintf(stderr, "[Database] ERROR: Failed to allocate memory for connection pool\n");
            return -1;
        }
        memset(pool->connections, 0, config->pool_size * sizeof(DBConnection));
    }

    // Create connections in the pool
    for (int i = pool->pool_size; i < config->pool_size; i++) {
        DBConnection* conn = &pool->connections[i];

        memset(conn, 0, sizeof(DBConnection));
        conn->type = db_type;
        conn->is_connected = false;
        conn->last_used = time(NULL);
        conn->use_count = 0;

        MYSQL* mysql_conn = mysql_init(NULL);
        if (!mysql_conn) {
            fprintf(stderr, "[Database] ERROR: Failed to initialize MySQL connection\n");
            return -1;
        }

        if (!mysql_real_connect(mysql_conn, config->host, config->user, config->password,
                                config->database, config->port, NULL, 0)) {
            fprintf(stderr, "[Database] ERROR: Failed to connect to database type %d: %s\n",
                    db_type, mysql_error(mysql_conn));
            mysql_close(mysql_conn);
            return -1;
        }

        conn->mysql_conn = mysql_conn;
        conn->is_connected = true;

        pool->pool_size++;
        pool->available++;
    }

    printf("[Database] Connected to database type %d with %d connections\n", db_type, pool->pool_size);
    return 0;
}

// Connect to all databases
int CDBConnector_ConnectAll(CDBConnector* self) {
    int result = 0;

    for (int i = 1; i <= DB_TYPE_MAX_INDEX; i++) {
        if (CDBConnector_Connect(self, i) != 0) {
            fprintf(stderr, "[Database] ERROR: Failed to connect to database type %d\n", i);
            result = -1;
        }
    }

    return result;
}

// Execute query on specific database type
int CDBConnector_ExecuteQuery(CDBConnector* self, int db_type, const char* query, QueryResult* result) {
    DBConnection* conn = GetConnectionFromPool(self, db_type);
    if (!conn) {
        return -1;
    }

    int ret = mysql_query(conn->mysql_conn, query);
    if (ret != 0) {
        fprintf(stderr, "[Database] ERROR: Failed to execute query: %s\n", mysql_error(conn->mysql_conn));
        self->failed_queries++;
        ReleaseConnectionToPool(self, conn);
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(conn->mysql_conn);
    if (!res) {
        fprintf(stderr, "[Database] ERROR: Failed to get result set: %s\n", mysql_error(conn->mysql_conn));
        self->failed_queries++;
        ReleaseConnectionToPool(self, conn);
        return -1;
    }

    result->result_set = res;
    result->row_count = mysql_num_rows(res);
    result->column_count = mysql_num_fields(res);
    result->current_row = 0;

    conn->use_count++;
    self->total_queries++;
    ReleaseConnectionToPool(self, conn);
    return result->row_count;
}

// Execute query on specific connection
int CDBConnector_ExecuteQueryDirect(DBConnection* conn, const char* query, QueryResult* result) {
    int ret = mysql_query(conn->mysql_conn, query);
    if (ret != 0) {
        fprintf(stderr, "[Database] ERROR: Failed to execute query: %s\n", mysql_error(conn->mysql_conn));
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(conn->mysql_conn);
    if (!res) {
        fprintf(stderr, "[Database] ERROR: Failed to get result set: %s\n", mysql_error(conn->mysql_conn));
        return -1;
    }

    result->result_set = res;
    result->row_count = mysql_num_rows(res);
    result->column_count = mysql_num_fields(res);
    result->current_row = 0;

    conn->use_count++;
    return result->row_count;
}

// Prepare statement (for performance)
MYSQL_STMT* CDBConnector_PrepareStatement(CDBConnector* self, int db_type, const char* query) {
    DBConnection* conn = GetConnectionFromPool(self, db_type);
    if (!conn) {
        return NULL;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn->mysql_conn);
    if (!stmt) {
        fprintf(stderr, "[Database] ERROR: Failed to initialize prepared statement: %s\n", mysql_error(conn->mysql_conn));
        ReleaseConnectionToPool(self, conn);
        return NULL;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        fprintf(stderr, "[Database] ERROR: Failed to prepare statement: %s\n", mysql_error(conn->mysql_conn));
        mysql_stmt_close(stmt);
        ReleaseConnectionToPool(self, conn);
        return NULL;
    }

    ReleaseConnectionToPool(self, conn);
    return stmt;
}

// Fetch next row from result set
int QueryResult_FetchRow(QueryResult* self, char** values) {
    MYSQL_ROW row = mysql_fetch_row(self->result_set);
    if (!row) {
        return 0;
    }

    for (int i = 0; i < self->column_count; i++) {
        values[i] = row[i];
    }

    self->current_row++;
    return self->column_count;
}

// Get field value by index
const char* QueryResult_GetField(QueryResult* self, int field_index) {
    MYSQL_ROW row = mysql_fetch_row(self->result_set);
    if (!row || field_index < 0 || field_index >= self->column_count) {
        return NULL;
    }
    return row[field_index];
}

// Get field value by name
const char* QueryResult_GetFieldByName(QueryResult* self, const char* field_name) {
    MYSQL_FIELD* fields = mysql_fetch_fields(self->result_set);
    for (int i = 0; i < self->column_count; i++) {
        if (strcmp(fields[i].name, field_name) == 0) {
            MYSQL_ROW row = mysql_fetch_row(self->result_set);
            if (!row) {
                return NULL;
            }
            return row[i];
        }
    }
    return NULL;
}

// Get affected rows
int QueryResult_GetAffectedRows(QueryResult* self) {
    if (!self->result_set) {
        return -1;
    }
    return self->row_count;
}

// Free result set
void QueryResult_Free(QueryResult* self) {
    if (self->result_set) {
        mysql_free_result(self->result_set);
        self->result_set = NULL;
    }
    self->row_count = 0;
    self->column_count = 0;
    self->current_row = 0;
}

// Cleanup and close all connections
void CDBConnector_Cleanup(CDBConnector* self) {
    if (!self->initialized) {
        return;
    }

    // Close all connections in all pools
    for (int i = 0; i <= DB_TYPE_MAX_INDEX; i++) {
        DBConnectionPool* pool = &self->pools[i];

        if (pool->connections) {
            for (int j = 0; j < pool->pool_size; j++) {
                if (pool->connections[j].mysql_conn) {
                    mysql_close(pool->connections[j].mysql_conn);
                    pool->connections[j].mysql_conn = NULL;
                    pool->connections[j].is_connected = false;
                }
            }

            free(pool->connections);
            pool->connections = NULL;
            pool->pool_size = 0;
            pool->available = 0;
            pool->in_use = 0;
        }
    }

    self->initialized = false;
    printf("[Database] All database connections closed successfully\n");
}

// Check if connected to specific database type
bool CDBConnector_IsConnected(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return false;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    return pool->pool_size > 0 && pool->connections != NULL;
}

// Get statistics for specific database type
void CDBConnector_GetStats(CDBConnector* self, int db_type, int* total_queries, int* failed_queries) {
    if (total_queries) {
        *total_queries = self->total_queries;
    }
    if (failed_queries) {
        *failed_queries = self->failed_queries;
    }
}

// Reconnect to specific database type
int CDBConnector_Reconnect(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    // Close existing connections first
    DBConnectionPool* pool = &self->pools[db_type];
    if (pool->connections) {
        for (int j = 0; j < pool->pool_size; j++) {
            if (pool->connections[j].mysql_conn) {
                mysql_close(pool->connections[j].mysql_conn);
            }
        }
        free(pool->connections);
        pool->connections = NULL;
        pool->pool_size = 0;
        pool->available = 0;
        pool->in_use = 0;
    }

    // Reconnect
    return CDBConnector_Connect(self, db_type);
}

// Get connection handle
MYSQL* CDBConnector_GetConnection(CDBConnector* self, int db_type) {
    DBConnection* conn = GetConnectionFromPool(self, db_type);
    if (!conn) {
        return NULL;
    }
    return conn->mysql_conn;
}

// Begin transaction
int CDBConnector_BeginTransaction(CDBConnector* self, int db_type) {
    QueryResult result;
    return CDBConnector_ExecuteQuery(self, db_type, "START TRANSACTION", &result);
}

// Commit transaction
int CDBConnector_CommitTransaction(CDBConnector* self, int db_type) {
    QueryResult result;
    return CDBConnector_ExecuteQuery(self, db_type, "COMMIT", &result);
}

// Rollback transaction
int CDBConnector_RollbackTransaction(CDBConnector* self, int db_type) {
    QueryResult result;
    return CDBConnector_ExecuteQuery(self, db_type, "ROLLBACK", &result);
}

// Get connection pool statistics
int CDBConnector_GetConnectionPoolStats(CDBConnector* self, int db_type, int* total, int* available, int* inuse) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    if (total) {
        *total = pool->pool_size;
    }
    if (available) {
        *available = pool->available;
    }
    if (inuse) {
        *inuse = pool->in_use;
    }

    return 0;
}