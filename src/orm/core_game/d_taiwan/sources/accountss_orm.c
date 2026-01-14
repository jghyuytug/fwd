#include "accountss_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048
#define MAX_FIELDS 8

/* ==================== CRUD 操作实现 ==================== */

int Accountss_Add(DBConnectionManager* manager, const Accountss* account) {
    char query[MAX_QUERY_LEN];
    char escaped_accountname[512];
    char escaped_password[512];
    DBQueryResult result;

    if (!manager || !account) {
        return -1;
    }

    /* 转义字符串（简化处理）*/
    snprintf(escaped_accountname, sizeof(escaped_accountname), "%s", account->accountname);
    snprintf(escaped_password, sizeof(escaped_password), "%s", account->password);

    snprintf(query, sizeof(query),
        "INSERT INTO accountss (accountname, password) VALUES ('%s', '%s')",
        escaped_accountname, escaped_password);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);

    /* 获取自增ID */
    memset(&result, 0, sizeof(DBQueryResult));
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int new_id = atoi(values[0]);
    DBQueryResult_Free(&result);
    return new_id;
}

int Accountss_Get(DBConnectionManager* manager, int uid, Accountss* account) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !account) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT uid, accountname, password FROM accountss WHERE uid = %d",
        uid);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[3];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    account->uid = atoi(values[0]);
    snprintf(account->accountname, sizeof(account->accountname), "%s", values[1] ? values[1] : "");
    snprintf(account->password, sizeof(account->password), "%s", values[2] ? values[2] : "");

    DBQueryResult_Free(&result);
    return 0;
}

int Accountss_GetByAccountname(DBConnectionManager* manager, const char* accountname, Accountss* account) {
    char query[MAX_QUERY_LEN];
    char escaped_accountname[512];
    DBQueryResult result;

    if (!manager || !accountname || !account) {
        return -1;
    }

    snprintf(escaped_accountname, sizeof(escaped_accountname), "%s", accountname);

    snprintf(query, sizeof(query),
        "SELECT uid, accountname, password FROM accountss WHERE accountname = '%s'",
        escaped_accountname);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[3];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    account->uid = atoi(values[0]);
    snprintf(account->accountname, sizeof(account->accountname), "%s", values[1] ? values[1] : "");
    snprintf(account->password, sizeof(account->password), "%s", values[2] ? values[2] : "");

    DBQueryResult_Free(&result);
    return 0;
}

int Accountss_Update(DBConnectionManager* manager, const Accountss* account) {
    char query[MAX_QUERY_LEN];
    char escaped_accountname[512];
    char escaped_password[512];
    DBQueryResult result;

    if (!manager || !account) {
        return -1;
    }

    snprintf(escaped_accountname, sizeof(escaped_accountname), "%s", account->accountname);
    snprintf(escaped_password, sizeof(escaped_password), "%s", account->password);

    snprintf(query, sizeof(query),
        "UPDATE accountss SET accountname='%s', password='%s' WHERE uid=%d",
        escaped_accountname, escaped_password, account->uid);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Accountss_Delete(DBConnectionManager* manager, int uid) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM accountss WHERE uid = %d", uid);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Accountss_Exists(DBConnectionManager* manager, int uid) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM accountss WHERE uid = %d LIMIT 1", uid);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    int exists = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

int Accountss_ExistsByAccountname(DBConnectionManager* manager, const char* accountname) {
    char query[MAX_QUERY_LEN];
    char escaped_accountname[512];
    DBQueryResult result;

    if (!manager || !accountname) {
        return -1;
    }

    snprintf(escaped_accountname, sizeof(escaped_accountname), "%s", accountname);

    snprintf(query, sizeof(query),
        "SELECT 1 FROM accountss WHERE accountname = '%s' LIMIT 1",
        escaped_accountname);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    int exists = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作实现 ==================== */

int Accountss_UpdatePassword(DBConnectionManager* manager, int uid, const char* new_password) {
    char query[MAX_QUERY_LEN];
    char escaped_password[512];
    DBQueryResult result;

    if (!manager || !new_password) {
        return -1;
    }

    snprintf(escaped_password, sizeof(escaped_password), "%s", new_password);

    snprintf(query, sizeof(query),
        "UPDATE accountss SET password='%s' WHERE uid=%d",
        escaped_password, uid);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Accountss_VerifyPassword(DBConnectionManager* manager, const char* accountname, const char* password) {
    char query[MAX_QUERY_LEN];
    char escaped_accountname[512];
    char escaped_password[512];
    DBQueryResult result;

    if (!manager || !accountname || !password) {
        return -1;
    }

    snprintf(escaped_accountname, sizeof(escaped_accountname), "%s", accountname);
    snprintf(escaped_password, sizeof(escaped_password), "%s", password);

    snprintf(query, sizeof(query),
        "SELECT 1 FROM accountss WHERE accountname='%s' AND password='%s' LIMIT 1",
        escaped_accountname, escaped_password);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    int verified = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return verified;
}

/* ==================== 查询操作实现 ==================== */

int Accountss_CountTotal(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM accountss");

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
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

int Accountss_CountByPattern(DBConnectionManager* manager, const char* pattern) {
    char query[MAX_QUERY_LEN];
    char escaped_pattern[512];
    DBQueryResult result;

    if (!manager || !pattern) {
        return -1;
    }

    snprintf(escaped_pattern, sizeof(escaped_pattern), "%s", pattern);

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM accountss WHERE accountname LIKE '%s'",
        escaped_pattern);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
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

/* ==================== 批量操作实现 ==================== */

int Accountss_BatchDelete(DBConnectionManager* manager, const int* uids, int count) {
    char query[MAX_QUERY_LEN];
    char uid_list[1024];
    DBQueryResult result;

    if (!manager || !uids || count <= 0) {
        return -1;
    }

    /* 构建 uid 列表 */
    uid_list[0] = '\0';
    for (int i = 0; i < count; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", uids[i]);
        if (i > 0) {
            strcat(uid_list, ",");
        }
        strcat(uid_list, buf);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM accountss WHERE uid IN (%s)", uid_list);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void Accountss_PrintInfo(const Accountss* account) {
    if (!account) {
        printf("Accountss: NULL\n");
        return;
    }

    printf("Accountss:\n");
    printf("  uid: %d\n", account->uid);
    printf("  accountname: %s\n", account->accountname);
    printf("  password: %s (masked)\n", account->password[0] ? "****" : "(empty)");
}

void Accountss_Free(Accountss* account) {
    /* 当前结构体没有动态分配的字段，此函数保留供将来扩展 */
    (void)account;
}
