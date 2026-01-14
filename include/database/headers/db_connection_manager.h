#ifndef DB_CONNECTION_MANAGER_H
#define DB_CONNECTION_MANAGER_H

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明 MySQL 类型 - 避免头文件冲突
struct st_mysql;
struct st_mysql_res;
struct st_mysql_stmt;

typedef struct st_mysql MYSQL_CONNECTION;
typedef struct st_mysql_res MYSQL_RESULT_SET;
typedef struct st_mysql_stmt MYSQL_STATEMENT;

// ========================================================================
// 数据库类型枚举 - 对应5个测试数据库
// ========================================================================
typedef enum {
    DB_TYPE_TAIWAN = 1,      // test_d_taiwan (账号数据库)
    DB_TYPE_CAIN = 2,        // test_taiwan_cain (游戏数据/角色数据)
    DB_TYPE_BILLING = 3,     // test_taiwan_billing (计费数据库)
    DB_TYPE_LOGIN = 4,       // test_taiwan_login (登录日志)
    DB_TYPE_GUILD = 5,       // test_d_guild (公会数据库)
    DB_TYPE_MAX_INDEX = 15    // 最大索引
} DatabaseType;

// ========================================================================
// 数据库配置结构
// ========================================================================
typedef struct {
    int type;                // 数据库类型
    char host[64];           // 主机地址
    int port;                // 端口号
    char user[32];           // 用户名
    char password[128];      // 密码
    char database[64];       // 数据库名
    int pool_size;           // 连接池大小
    int timeout;             // 连接超时时间(秒)
} DatabaseConfig;

// ========================================================================
// 数据库连接结构
// ========================================================================
typedef struct {
    int type;                      // 数据库类型
    MYSQL_CONNECTION* mysql_conn;  // MySQL连接句柄
    bool is_connected;             // 是否已连接
    bool in_use;                   // 是否正在使用中
    time_t last_used;              // 最后使用时间
    int use_count;                 // 使用次数
} DatabaseConnection;

// ========================================================================
// 连接池结构
// ========================================================================
typedef struct {
    DatabaseConnection* connections; // 连接数组
    int pool_size;                   // 当前连接数
    int available;                   // 可用连接数
    int max_size;                    // 最大连接数
    int in_use;                      // 正在使用的连接数
} ConnectionPool;

// ========================================================================
// 数据库管理器主结构
// ========================================================================
typedef struct {
    DatabaseConfig configs[DB_TYPE_MAX_INDEX + 1];  // 数据库配置数组
    ConnectionPool pools[DB_TYPE_MAX_INDEX + 1];    // 连接池数组
    int total_queries;          // 总查询次数
    int failed_queries;         // 失败查询次数
    int connection_errors;      // 连接错误次数
    bool initialized;           // 是否已初始化
} DBConnectionManager;

// ========================================================================
// 查询结果结构
// ========================================================================
typedef struct {
    MYSQL_RESULT_SET* result_set;  // MySQL结果集
    int row_count;                 // 行数
    int column_count;              // 列数
    int current_row;               // 当前行索引
} DBQueryResult;

// ========================================================================
// 数据库管理器函数声明
// ========================================================================

// 初始化数据库管理器
int DBConnectionManager_Initialize(DBConnectionManager* self, const char* config_file);

// 连接到指定类型的数据库
int DBConnectionManager_Connect(DBConnectionManager* self, int db_type);

// 连接到所有数据库
int DBConnectionManager_ConnectAll(DBConnectionManager* self);

// 执行查询
int DBConnectionManager_ExecuteQuery(DBConnectionManager* self, int db_type, const char* query, DBQueryResult* result);

// 在特定连接上执行查询
int DBConnectionManager_ExecuteQueryDirect(DatabaseConnection* conn, const char* query, DBQueryResult* result);

// INSERT helper returning insert_id
int DBConnectionManager_ExecuteInsert(DBConnectionManager* self, int db_type, const char* query, unsigned long long* insert_id);

// 准备预处理语句
MYSQL_STATEMENT* DBConnectionManager_PrepareStatement(DBConnectionManager* self, int db_type, const char* query);

// 清理并关闭所有连接
void DBConnectionManager_Cleanup(DBConnectionManager* self);

// 检查是否已连接到指定数据库
bool DBConnectionManager_IsConnected(DBConnectionManager* self, int db_type);

// 获取统计信息
void DBConnectionManager_GetStats(DBConnectionManager* self, int db_type, int* total_queries, int* failed_queries);

// 重新连接到指定数据库
int DBConnectionManager_Reconnect(DBConnectionManager* self, int db_type);

// 获取连接句柄
MYSQL_CONNECTION* DBConnectionManager_GetConnection(DBConnectionManager* self, int db_type);

// 事务管理
int DBConnectionManager_BeginTransaction(DBConnectionManager* self, int db_type);
int DBConnectionManager_CommitTransaction(DBConnectionManager* self, int db_type);
int DBConnectionManager_RollbackTransaction(DBConnectionManager* self, int db_type);

// 获取连接池统计信息
int DBConnectionManager_GetConnectionPoolStats(DBConnectionManager* self, int db_type, int* total, int* available, int* inuse);

// ========================================================================
// 查询结果函数声明
// ========================================================================

// 获取下一行
int DBQueryResult_FetchRow(DBQueryResult* self, char** values);

// 获取下一行（带字段长度信息）- 用于BLOB字段
int DBQueryResult_FetchRowWithLengths(DBQueryResult* self, char** values, unsigned long** lengths);

// 按索引获取字段值
const char* DBQueryResult_GetField(DBQueryResult* self, int field_index);

// 按名称获取字段值
const char* DBQueryResult_GetFieldByName(DBQueryResult* self, const char* field_name);

// 获取影响的行数
int DBQueryResult_GetAffectedRows(DBQueryResult* self);

// 释放结果集
void DBQueryResult_Free(DBQueryResult* self);

#ifdef __cplusplus
}
#endif

#endif // DB_CONNECTION_MANAGER_H
