#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <stdbool.h>
#include <time.h>

// 前向声明 - 避免与其他头文件的类型冲突
typedef struct st_mysql MYSQL;
typedef struct st_mysql_res MYSQL_RES;
typedef struct st_mysql_stmt MYSQL_STMT;

// ========================================================================
// 数据库类型枚举 - 对应5个测试数据库
// ========================================================================
typedef enum {
    DB_TYPE_TAIWAN = 1,      // test_d_taiwan (账号数据库)
    DB_TYPE_CAIN = 2,        // test_taiwan_cain (游戏数据/角色数据)
    DB_TYPE_BILLING = 3,     // test_taiwan_billing (计费数据库)
    DB_TYPE_LOGIN = 4,       // test_taiwan_login (登录日志)
    DB_TYPE_GUILD = 5,       // test_d_guild (公会数据库)
    DB_TYPE_MAX_INDEX = 5    // 最大索引
} DBType;

// ========================================================================
// 数据库配置结构
// ========================================================================
typedef struct {
    int type;                // 数据库类型 (DBType)
    char host[64];           // 主机地址
    int port;                // 端口号
    char user[32];           // 用户名
    char password[128];      // 密码
    char database[64];       // 数据库名
    int pool_size;           // 连接池大小
    int timeout;             // 连接超时时间(秒)
} DBConfig;

// ========================================================================
// 数据库连接结构
// ========================================================================
typedef struct {
    int type;                // 数据库类型
    MYSQL* mysql_conn;       // MySQL连接句柄
    bool is_connected;       // 是否已连接
    time_t last_used;        // 最后使用时间
    int use_count;           // 使用次数
} DBConnection;

// ========================================================================
// 连接池结构
// ========================================================================
typedef struct {
    DBConnection* connections;  // 连接数组
    int pool_size;              // 当前连接数
    int available;              // 可用连接数
    int max_size;               // 最大连接数
    int in_use;                 // 正在使用的连接数
} DBConnectionPool;

// ========================================================================
// 数据库连接器主结构
// ========================================================================
typedef struct {
    DBConfig configs[DB_TYPE_MAX_INDEX + 1];      // 数据库配置数组
    DBConnectionPool pools[DB_TYPE_MAX_INDEX + 1]; // 连接池数组
    int total_queries;          // 总查询次数
    int failed_queries;         // 失败查询次数
    int connection_errors;      // 连接错误次数
    bool initialized;           // 是否已初始化
} CDBConnector;

// ========================================================================
// 查询结果结构
// ========================================================================
typedef struct {
    MYSQL_RES* result_set;      // MySQL结果集
    int row_count;              // 行数
    int column_count;           // 列数
    int current_row;            // 当前行索引
} QueryResult;

// ========================================================================
// 数据库连接器函数声明
// ========================================================================

// 初始化数据库连接器
int CDBConnector_Initialize(CDBConnector* self, const char* config_file);

// 连接到指定类型的数据库
int CDBConnector_Connect(CDBConnector* self, int db_type);

// 连接到所有数据库
int CDBConnector_ConnectAll(CDBConnector* self);

// 执行查询
int CDBConnector_ExecuteQuery(CDBConnector* self, int db_type, const char* query, QueryResult* result);

// 在特定连接上执行查询
int CDBConnector_ExecuteQueryDirect(DBConnection* conn, const char* query, QueryResult* result);

// 准备预处理语句
MYSQL_STMT* CDBConnector_PrepareStatement(CDBConnector* self, int db_type, const char* query);

// 清理并关闭所有连接
void CDBConnector_Cleanup(CDBConnector* self);

// 检查是否已连接到指定数据库
bool CDBConnector_IsConnected(CDBConnector* self, int db_type);

// 获取统计信息
void CDBConnector_GetStats(CDBConnector* self, int db_type, int* total_queries, int* failed_queries);

// 重新连接到指定数据库
int CDBConnector_Reconnect(CDBConnector* self, int db_type);

// 获取连接句柄
MYSQL* CDBConnector_GetConnection(CDBConnector* self, int db_type);

// 事务管理
int CDBConnector_BeginTransaction(CDBConnector* self, int db_type);
int CDBConnector_CommitTransaction(CDBConnector* self, int db_type);
int CDBConnector_RollbackTransaction(CDBConnector* self, int db_type);

// 获取连接池统计信息
int CDBConnector_GetConnectionPoolStats(CDBConnector* self, int db_type, int* total, int* available, int* inuse);

// ========================================================================
// 查询结果函数声明
// ========================================================================

// 获取下一行
int QueryResult_FetchRow(QueryResult* self, char** values);

// 按索引获取字段值
const char* QueryResult_GetField(QueryResult* self, int field_index);

// 按名称获取字段值
const char* QueryResult_GetFieldByName(QueryResult* self, const char* field_name);

// 获取影响的行数
int QueryResult_GetAffectedRows(QueryResult* self);

// 释放结果集
void QueryResult_Free(QueryResult* self);

#endif // DB_MANAGER_H
