#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <mysql/mysql.h>

// Database type definitions
#define DB_TYPE_ACCOUNT_MASTER      1   // d_taiwan
#define DB_TYPE_CHARACTER_PRIMARY   2   // taiwan_cain
#define DB_TYPE_CHARACTER_SECONDARY 3   // taiwan_cain_2nd
#define DB_TYPE_GAME_LOG            4   // taiwan_cain_log
#define DB_TYPE_LOGIN_SESSION       6   // taiwan_login
#define DB_TYPE_PRODUCTION          7   // taiwan_prod
#define DB_TYPE_GUILD               8   // d_guild
#define DB_TYPE_GAME_EVENT          9   // taiwan_game_event
#define DB_TYPE_SECURITY           10   // d_taiwan_secu
#define DB_TYPE_LOGIN_PLAY         11   // taiwan_login_play
#define DB_TYPE_AUCTION_GOLD       12   // taiwan_cain_auction_gold
#define DB_TYPE_SPECIAL_EVENT      13   // taiwan_se_event
#define DB_TYPE_BILLING            14   // taiwan_billing
#define DB_TYPE_MAX_INDEX          14   // Maximum index (14)

// Database connection configuration
typedef struct {
    int type;           // DB type (1-14)
    char host[64];      // Server IP
    int port;           // Server port
    char user[32];      // Username
    char password[128]; // Password (encrypted)
    char database[64];  // Database name
    int pool_size;      // Connection pool size
    int timeout;        // Connection timeout (seconds)
} DBConfig;

// Database connection handle
typedef struct {
    int type;           // Database type identifier
    MYSQL* mysql_conn;  // MySQL connection handle
    bool is_connected;  // Connection state
    int last_used;      // Timestamp of last use
    int use_count;      // Query count

    // Phase 5.9b: Health monitoring
    int created_at;     // Timestamp when connection was created
    int last_health_check; // Last health check timestamp
    bool is_healthy;    // Health status
    int failed_queries; // Failed query count for this connection
    int reconnect_count; // Number of reconnections
    bool in_use;        // Currently being used
} DBConnection;

// Connection pool
typedef struct {
    DBConnection* connections;  // Array of connections
    int pool_size;              // Current pool size
    int available;              // Available connections
    int max_size;               // Maximum size
    int in_use;                 // Number in use

    // Phase 5.9b: Performance metrics
    int total_connections_created;  // Total connections ever created
    int total_connections_destroyed; // Total connections destroyed
    int peak_connections;       // Peak number of connections
    int connection_wait_count;  // Times had to wait for connection
    long long total_query_time; // Total query execution time (ms)
    int health_check_failures;  // Failed health checks
    int auto_reconnects;        // Automatic reconnections
} DBConnectionPool;

// Transaction state tracking (Phase 5.9c)
typedef struct {
    int db_type;                // Database type
    DBConnection* connection;   // Connection handling this transaction
    bool is_active;             // Transaction active flag
    int started_at;             // Transaction start time
    int savepoint_depth;        // Nested savepoint depth (for nested transactions)
    char savepoint_name[64];    // Current savepoint name
    int timeout_seconds;        // Transaction-specific timeout
} TransactionState;

// Database connector (main manager)
typedef struct {
    DBConfig configs[DB_TYPE_MAX_INDEX + 1]; // Configurations for types 1-14
    DBConnectionPool pools[DB_TYPE_MAX_INDEX + 1]; // Connection pools
    bool initialized;               // Initialization state

    // Statistics
    int total_queries;
    int failed_queries;
    int connection_errors;

    // Phase 5.9c: Query execution configuration
    int query_timeout_seconds;      // Query timeout (0 = no timeout)
    int max_retry_attempts;         // Maximum retry attempts
    int slow_query_threshold_ms;    // Slow query threshold in milliseconds
    bool enable_query_cache;        // Enable query result caching
    int cache_ttl_seconds;          // Cache time-to-live in seconds
    int cache_max_entries;          // Maximum cache entries

    // Phase 5.9c: Query execution statistics
    int slow_queries_detected;      // Number of slow queries detected
    int queries_retried;            // Number of queries retried
    int cache_hits;                 // Cache hit count
    int cache_misses;               // Cache miss count

    // Phase 5.9c: Enhanced transaction management
    int transaction_timeout_seconds;    // Transaction timeout (0 = no timeout)
    int transactions_started;           // Total transactions started
    int transactions_committed;         // Total transactions committed
    int transactions_rolled_back;       // Total transactions rolled back
    int transactions_timed_out;         // Total transactions that timed out
    TransactionState active_transactions[DB_TYPE_MAX_INDEX + 1]; // Active transaction per DB type
} CDBConnector;

// Query result handle
typedef struct {
    int row_count;
    int column_count;
    MYSQL_RES* result_set;   // MySQL result set
    int current_row;

    // Phase 5.9c: Query execution metrics
    long long execution_time_ms;  // Query execution time in milliseconds
    bool is_cached;               // Whether result is from cache
    int retry_count;              // Number of retries
    char query_hash[65];          // SHA256 hash of query for caching
} QueryResult;

// Initialization
int CDBConnector_Initialize(CDBConnector* self, const char* config_file);
int CDBConnector_Connect(CDBConnector* self, int db_type);
int CDBConnector_ConnectAll(CDBConnector* self);
void CDBConnector_Cleanup(CDBConnector* self);

// Query execution
int CDBConnector_ExecuteQuery(CDBConnector* self, int db_type, const char* query, QueryResult* result);
int CDBConnector_ExecuteQueryDirect(DBConnection* conn, const char* query, QueryResult* result);
MYSQL_STMT* CDBConnector_PrepareStatement(CDBConnector* self, int db_type, const char* query);

// Result processing
int QueryResult_FetchRow(QueryResult* self, char** values);
const char* QueryResult_GetField(QueryResult* self, int field_index);
const char* QueryResult_GetFieldByName(QueryResult* self, const char* field_name);
int QueryResult_GetAffectedRows(QueryResult* self);
void QueryResult_Free(QueryResult* self);

// Connection management
bool CDBConnector_IsConnected(CDBConnector* self, int db_type);
void CDBConnector_GetStats(CDBConnector* self, int db_type, int* total_queries, int* failed_queries);
int CDBConnector_Reconnect(CDBConnector* self, int db_type);
MYSQL* CDBConnector_GetConnection(CDBConnector* self, int db_type);

// New functions for Phase 5.9
int CDBConnector_BeginTransaction(CDBConnector* self, int db_type);
int CDBConnector_CommitTransaction(CDBConnector* self, int db_type);
int CDBConnector_RollbackTransaction(CDBConnector* self, int db_type);
int CDBConnector_GetConnectionPoolStats(CDBConnector* self, int db_type, int* total, int* available, int* inuse);

// Phase 5.9c: Enhanced transaction management
void CDBConnector_SetTransactionTimeout(CDBConnector* self, int timeout_seconds);
int CDBConnector_BeginTransactionWithTimeout(CDBConnector* self, int db_type, int timeout_seconds);
int CDBConnector_CreateSavepoint(CDBConnector* self, int db_type, const char* savepoint_name);
int CDBConnector_RollbackToSavepoint(CDBConnector* self, int db_type, const char* savepoint_name);
int CDBConnector_ReleaseSavepoint(CDBConnector* self, int db_type, const char* savepoint_name);
int CDBConnector_CheckTransactionTimeout(CDBConnector* self, int db_type);
void CDBConnector_PrintTransactionStats(CDBConnector* self);
int CDBConnector_GetTransactionStats(CDBConnector* self, int* started, int* committed,
                                      int* rolled_back, int* timed_out);


// Phase 5.9b: Advanced connection pool management
int CDBConnector_CheckConnectionHealth(CDBConnector* self, DBConnection* conn);
int CDBConnector_HealthCheckAll(CDBConnector* self, int db_type);
int CDBConnector_PrewarmPool(CDBConnector* self, int db_type);
int CDBConnector_DetectConnectionTimeout(CDBConnector* self, int db_type, int timeout_seconds);
int CDBConnector_DetectConnectionLeaks(CDBConnector* self, int db_type);
void CDBConnector_PrintPoolMetrics(CDBConnector* self, int db_type);
int CDBConnector_GetDetailedStats(CDBConnector* self, int db_type,
                                   int* total_created, int* total_destroyed,
                                   int* peak_connections, int* wait_count,
                                   long long* total_query_time);

// Phase 5.9c: Advanced query execution and result processing
void CDBConnector_SetQueryTimeout(CDBConnector* self, int timeout_seconds);
void CDBConnector_SetMaxRetryAttempts(CDBConnector* self, int max_attempts);
void CDBConnector_SetSlowQueryThreshold(CDBConnector* self, int threshold_ms);
void CDBConnector_EnableQueryCache(CDBConnector* self, bool enable, int ttl_seconds, int max_entries);

int CDBConnector_ExecuteQueryWithTimeout(CDBConnector* self, int db_type, const char* query,
                                          QueryResult* result, int timeout_seconds);
int CDBConnector_ExecuteQueryWithRetry(CDBConnector* self, int db_type, const char* query,
                                        QueryResult* result, int max_retries);
int CDBConnector_ExecuteQueryCached(CDBConnector* self, int db_type, const char* query,
                                     QueryResult* result);

void CDBConnector_DetectSlowQueries(CDBConnector* self);
void CDBConnector_PrintSlowQueryReport(CDBConnector* self);
void CDBConnector_ClearQueryCache(CDBConnector* self);
void CDBConnector_GetCacheStats(CDBConnector* self, int* hits, int* misses, float* hit_rate);

// Streaming result set processing
int QueryResult_InitStream(QueryResult* self, MYSQL_RES* result_set);
int QueryResult_FetchNextBatch(QueryResult* self, int batch_size, char*** rows);
void QueryResult_FreeStream(QueryResult* self);