/**
 * DNF游戏服务器 - LoginLog表ORM实现
 */

#define _GNU_SOURCE
#include "login_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 获取当前时间字符串 (YYYY-MM-DD HH:MM:SS)
 */
static void GetCurrentTimeString(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

/**
 * 从查询结果行填充登录日志结构
 */
static void FillLoginLogFromRow(char** values, LoginLog* log) {
    log->seq = strtoll(values[0], NULL, 10);
    log->account_id = atoi(values[1]);
    strncpy(log->login_time, values[2], sizeof(log->login_time) - 1);
    strncpy(log->logout_time, values[3], sizeof(log->logout_time) - 1);
    strncpy(log->login_ip, values[4], sizeof(log->login_ip) - 1);
    log->server_id = atoi(values[5]);
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int LoginLog_Create(DBConnectionManager* manager, LoginLog* log) {
    char query[512];
    char current_time[20];

    // 获取当前时间作为登录时间
    GetCurrentTimeString(current_time, sizeof(current_time));
    strncpy(log->login_time, current_time, sizeof(log->login_time) - 1);

    // 如果没有设置登出时间，使用默认值
    if (strlen(log->logout_time) == 0) {
        strcpy(log->logout_time, "0000-00-00 00:00:00");
    }

    // 构建INSERT查询 (seq是自增的，不需要指定)
    snprintf(query, sizeof(query),
             "INSERT INTO login_log (account_id, login_time, logout_time, login_ip, server_id) "
             "VALUES (%d, '%s', '%s', '%s', %d)",
             log->account_id, log->login_time, log->logout_time,
             log->login_ip, log->server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        printf("[LoginLog ORM] Created login log for account_id=%d, ip=%s\n",
               log->account_id, log->login_ip);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int LoginLog_GetBySeq(DBConnectionManager* manager, long long seq, LoginLog* log) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log WHERE seq = %lld",
             seq);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    // 使用DBQueryResult_FetchRow获取行数据
    char* values[6];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillLoginLogFromRow(values, log);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int LoginLog_Update(DBConnectionManager* manager, const LoginLog* log) {
    char query[512];

    snprintf(query, sizeof(query),
             "UPDATE login_log SET account_id=%d, login_time='%s', "
             "logout_time='%s', login_ip='%s', server_id=%d WHERE seq=%lld",
             log->account_id, log->login_time, log->logout_time,
             log->login_ip, log->server_id, log->seq);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        printf("[LoginLog ORM] Updated login log seq=%lld\n", log->seq);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int LoginLog_Delete(DBConnectionManager* manager, long long seq) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM login_log WHERE seq=%lld", seq);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret >= 0) {  // DELETE can affect 0 rows if record doesn't exist
        printf("[LoginLog ORM] Deleted login log seq=%lld\n", seq);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

// ========================================================================
// 登录日志专用操作函数实现
// ========================================================================

int LoginLog_UpdateLogoutTime(DBConnectionManager* manager, long long seq, const char* logout_time) {
    char query[256];

    snprintf(query, sizeof(query),
             "UPDATE login_log SET logout_time='%s' WHERE seq=%lld",
             logout_time, seq);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        printf("[LoginLog ORM] Updated logout time for seq=%lld\n", seq);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int LoginLog_GetByAccountId(DBConnectionManager* manager, int account_id,
                            LoginLog* logs, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log WHERE account_id = %d "
             "ORDER BY login_time DESC LIMIT %d",
             account_id, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[6];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillLoginLogFromRow(values, &logs[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int LoginLog_GetRecentByAccountId(DBConnectionManager* manager, int account_id,
                                  int limit, LoginLog* logs) {
    return LoginLog_GetByAccountId(manager, account_id, logs, limit);
}

int LoginLog_GetByIP(DBConnectionManager* manager, const char* login_ip,
                     LoginLog* logs, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log WHERE login_ip = '%s' "
             "ORDER BY login_time DESC LIMIT %d",
             login_ip, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[6];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillLoginLogFromRow(values, &logs[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int LoginLog_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                            const char* end_time, LoginLog* logs, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log WHERE login_time >= '%s' AND login_time <= '%s' "
             "ORDER BY login_time DESC LIMIT %d",
             start_time, end_time, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[6];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillLoginLogFromRow(values, &logs[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int LoginLog_GetActiveLogins(DBConnectionManager* manager, LoginLog* logs, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log WHERE logout_time = '0000-00-00 00:00:00' "
             "ORDER BY login_time DESC LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[6];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillLoginLogFromRow(values, &logs[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int LoginLog_GetByServerId(DBConnectionManager* manager, int server_id,
                           LoginLog* logs, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log WHERE server_id = %d "
             "ORDER BY login_time DESC LIMIT %d",
             server_id, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[6];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillLoginLogFromRow(values, &logs[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 辅助查询函数实现
// ========================================================================

bool LoginLog_Exists(DBConnectionManager* manager, long long seq) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM login_log WHERE seq=%lld", seq);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (count > 0);
}

int LoginLog_GetCount(DBConnectionManager* manager) {
    char query[128];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM login_log");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

int LoginLog_GetCountByAccountId(DBConnectionManager* manager, int account_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM login_log WHERE account_id=%d", account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

int LoginLog_GetAll(DBConnectionManager* manager, LoginLog* logs, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT seq, account_id, login_time, logout_time, login_ip, server_id "
             "FROM login_log "
             "ORDER BY login_time DESC LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[6];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillLoginLogFromRow(values, &logs[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int LoginLog_DeleteBefore(DBConnectionManager* manager, const char* before_time) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM login_log WHERE login_time < '%s'", before_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret >= 0) {
        printf("[LoginLog ORM] Deleted %d old login logs before %s\n", ret, before_time);
        return ret;  // Return number of deleted rows
    } else {
        return -1;  // Failure
    }

    DBQueryResult_Free(&result);
}

// ========================================================================
// 工具函数实现
// ========================================================================

void LoginLog_Init(LoginLog* log) {
    memset(log, 0, sizeof(LoginLog));
    log->seq = 0;
    log->account_id = 0;
    log->server_id = 0;
    strcpy(log->login_time, "0000-00-00 00:00:00");
    strcpy(log->logout_time, "0000-00-00 00:00:00");
    strcpy(log->login_ip, "0.0.0.0");
}

void LoginLog_Print(const LoginLog* log) {
    printf("================================================================================\n");
    printf("LoginLog Information:\n");
    printf("================================================================================\n");
    printf("  序列号:        %lld\n", log->seq);
    printf("  账号ID:        %d\n", log->account_id);
    printf("  登录时间:      %s\n", log->login_time);
    printf("  登出时间:      %s\n", log->logout_time);
    printf("  登录IP:        %s\n", log->login_ip);
    printf("  服务器ID:      %d\n", log->server_id);
    printf("================================================================================\n");
}

void LoginLog_Copy(LoginLog* dest, const LoginLog* src) {
    dest->seq = src->seq;
    dest->account_id = src->account_id;
    dest->server_id = src->server_id;
    strncpy(dest->login_time, src->login_time, sizeof(dest->login_time) - 1);
    strncpy(dest->logout_time, src->logout_time, sizeof(dest->logout_time) - 1);
    strncpy(dest->login_ip, src->login_ip, sizeof(dest->login_ip) - 1);
}
