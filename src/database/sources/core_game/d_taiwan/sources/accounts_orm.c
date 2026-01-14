/**
 * DNF游戏服务器 - Accounts表ORM实现
 */

#define _GNU_SOURCE
#include "accounts_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 从查询结果行填充账号结构
 */
static void FillAccountFromRow(char** values, Account* account) {
    account->UID = atoi(values[0]);
    strncpy(account->accountname, values[1], sizeof(account->accountname) - 1);
    strncpy(account->password, values[2], sizeof(account->password) - 1);
    if (values[3] && strlen(values[3]) > 0) {
        strncpy(account->qq, values[3], sizeof(account->qq) - 1);
    } else {
        strcpy(account->qq, "");
    }
    account->dzuid = (values[4] && strlen(values[4]) > 0) ? atoi(values[4]) : 0;
    account->billing = (values[5] && strlen(values[5]) > 0) ? atoi(values[5]) : 0;
    strncpy(account->VIP, values[6], sizeof(account->VIP) - 1);
}

// ========================================================================
// CRUD 操作函数
// ========================================================================

/**
 * 创建新的账号
 */
int Account_Create(DBConnectionManager* manager, Account* account) {
    char query[2048];
    DBQueryResult result;
    int ret;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "INSERT INTO accounts (accountname, password, qq, dzuid, billing, VIP) "
             "VALUES ('%s', '%s', '%s', %d, %d, '%s')",
             account->accountname, account->password, account->qq,
             account->dzuid, account->billing, account->VIP);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    return (ret > 0) ? 0 : -1;
}

/**
 * 根据UID查询账号
 */
int Account_GetByUID(DBConnectionManager* manager, int UID, Account* account) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[7];

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, qq, dzuid, billing, VIP "
             "FROM accounts WHERE UID=%d",
             UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, account);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/**
 * 根据账号名查询账号
 */
int Account_GetByName(DBConnectionManager* manager, const char* accountname, Account* account) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[7];

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, qq, dzuid, billing, VIP "
             "FROM accounts WHERE accountname='%s'",
             accountname);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, account);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/**
 * 更新账号信息
 */
int Account_Update(DBConnectionManager* manager, const Account* account) {
    char query[2048];
    DBQueryResult result;
    int ret;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "UPDATE accounts SET accountname='%s', password='%s', qq='%s', "
             "dzuid=%d, billing=%d, VIP='%s' WHERE UID=%d",
             account->accountname, account->password, account->qq,
             account->dzuid, account->billing, account->VIP, account->UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    return (ret > 0) ? 0 : -1;
}

/**
 * 删除账号
 */
int Account_Delete(DBConnectionManager* manager, int UID) {
    char query[512];
    DBQueryResult result;
    int ret;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query), "DELETE FROM accounts WHERE UID=%d", UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    return (ret > 0) ? 0 : -1;
}

// ========================================================================
// 账号专用操作函数
// ========================================================================

/**
 * 验证账号密码
 */
int Account_Verify(DBConnectionManager* manager, const char* accountname, const char* password) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[1];
    int uid = -1;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID FROM accounts WHERE accountname='%s' AND password='%s'",
             accountname, password);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        uid = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return uid;
}

/**
 * 更新密码
 */
int Account_UpdatePassword(DBConnectionManager* manager, int UID, const char* new_password) {
    char query[1024];
    DBQueryResult result;
    int ret;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "UPDATE accounts SET password='%s' WHERE UID=%d",
             new_password, UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    return (ret > 0) ? 0 : -1;
}

/**
 * 更新VIP状态
 */
int Account_UpdateVIP(DBConnectionManager* manager, int UID, const char* vip_status) {
    char query[1024];
    DBQueryResult result;
    int ret;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "UPDATE accounts SET VIP='%s' WHERE UID=%d",
             vip_status, UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    return (ret > 0) ? 0 : -1;
}

/**
 * 更新QQ号
 */
int Account_UpdateQQ(DBConnectionManager* manager, int UID, const char* qq) {
    char query[1024];
    DBQueryResult result;
    int ret;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "UPDATE accounts SET qq='%s' WHERE UID=%d",
             qq, UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    return (ret > 0) ? 0 : -1;
}

/**
 * 根据QQ号查询账号
 */
int Account_GetByQQ(DBConnectionManager* manager, const char* qq,
                    Account* accounts, int max_count) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[7];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, qq, dzuid, billing, VIP "
             "FROM accounts WHERE qq='%s' LIMIT %d",
             qq, max_count);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 检查账号名是否已存在
 */
bool Account_NameExists(DBConnectionManager* manager, const char* accountname) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[1];
    bool exists = false;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM accounts WHERE accountname='%s'",
             accountname);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && atoi(values[0]) > 0) {
        exists = true;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/**
 * 根据VIP状态查询账号
 */
int Account_GetByVIP(DBConnectionManager* manager, const char* vip_status,
                     Account* accounts, int max_count) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[7];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, qq, dzuid, billing, VIP "
             "FROM accounts WHERE VIP='%s' LIMIT %d",
             vip_status, max_count);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查账号是否存在
 */
bool Account_Exists(DBConnectionManager* manager, int UID) {
    char query[512];
    DBQueryResult result;
    int ret;
    char* values[1];
    bool exists = false;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM accounts WHERE UID=%d",
             UID);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && atoi(values[0]) > 0) {
        exists = true;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/**
 * 获取账号总数
 */
int Account_GetCount(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;
    int ret;
    char* values[1];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM accounts");

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 获取所有账号
 */
int Account_GetAll(DBConnectionManager* manager, Account* accounts, int max_count) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[7];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, qq, dzuid, billing, VIP "
             "FROM accounts LIMIT %d",
             max_count);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 搜索账号（模糊匹配）
 */
int Account_Search(DBConnectionManager* manager, const char* keyword,
                   Account* accounts, int max_count) {
    char query[1024];
    DBQueryResult result;
    int ret;
    char* values[7];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, qq, dzuid, billing, VIP "
             "FROM accounts WHERE accountname LIKE '%%%s%%' OR qq LIKE '%%%s%%' "
             "LIMIT %d",
             keyword, keyword, max_count);

    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化Account结构体
 */
void Account_Init(Account* account) {
    memset(account, 0, sizeof(Account));
    account->UID = 0;
    strcpy(account->accountname, "");
    strcpy(account->password, "");
    strcpy(account->qq, "");
    account->dzuid = 0;
    account->billing = 0;
    strcpy(account->VIP, "");
}

/**
 * 打印Account信息（用于调试，密码将被隐藏）
 */
void Account_Print(const Account* account) {
    printf("Account Information:\n");
    printf("  UID: %d\n", account->UID);
    printf("  Account Name: %s\n", account->accountname);
    printf("  Password: ******** (hidden)\n");
    printf("  QQ: %s\n", account->qq);
    printf("  DZUID: %d\n", account->dzuid);
    printf("  Billing: %d\n", account->billing);
    printf("  VIP: %s\n", account->VIP);
}

/**
 * 复制Account结构体
 */
void Account_Copy(Account* dest, const Account* src) {
    memcpy(dest, src, sizeof(Account));
}
