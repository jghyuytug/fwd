#define _GNU_SOURCE
#include "../../include/database/headers/member_white_account_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FIELDS 20

/* ========================================================================
 * 辅助函数
 * ======================================================================== */

/**
 * 从查询结果行填充白名单账号结构体
 */
static void FillAccountFromRow(char** row, MemberWhiteAccount* account) {
    account->m_id = (unsigned int)atoi(row[0]);
    strncpy(account->reg_date, row[1] ? row[1] : "", sizeof(account->reg_date) - 1);
    account->reg_date[sizeof(account->reg_date) - 1] = '\0';
}

/* ========================================================================
 * 初始化函数
 * ======================================================================== */

void MemberWhiteAccount_Init(MemberWhiteAccount* account) {
    if (!account) return;
    memset(account, 0, sizeof(MemberWhiteAccount));
}

/* ========================================================================
 * CRUD操作
 * ======================================================================== */

int MemberWhiteAccount_Add(DBConnectionManager* manager, MemberWhiteAccount* account) {
    char query[512];

    if (!manager || !account) {
        return -1;
    }

    /* 使用REPLACE INTO实现幂等添加 */
    snprintf(query, sizeof(query),
             "REPLACE INTO member_white_account (m_id, reg_date) VALUES (%u, '%s')",
             account->m_id, account->reg_date);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret > 0) {
        printf("[MemberWhiteAccount ORM] Added whitelist: m_id=%u, reg_date=%s\n",
               account->m_id, account->reg_date);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberWhiteAccount_Get(DBConnectionManager* manager, unsigned int m_id, MemberWhiteAccount* account) {
    char query[256];

    if (!manager || !account) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, reg_date FROM member_white_account WHERE m_id = %u",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillAccountFromRow(values, account);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int MemberWhiteAccount_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[256];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM member_white_account WHERE m_id = %u",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    DBQueryResult_Free(&result);

    /* 幂等性：无论是否真的删除了记录，都返回0 */
    return 0;
}

bool MemberWhiteAccount_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[256];

    if (!manager) {
        return false;
    }

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_white_account WHERE m_id = %u",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[MAX_FIELDS];
    bool exists = false;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ========================================================================
 * 业务查询
 * ======================================================================== */

int MemberWhiteAccount_GetAll(DBConnectionManager* manager, MemberWhiteAccount* accounts, int offset, int limit) {
    char query[512];
    int count = 0;

    if (!manager || !accounts || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, reg_date FROM member_white_account ORDER BY reg_date DESC LIMIT %d OFFSET %d",
             limit, offset);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberWhiteAccount_GetCount(DBConnectionManager* manager) {
    char query[256];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_white_account");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int total_count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        total_count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return total_count;
}

int MemberWhiteAccount_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                                       MemberWhiteAccount* accounts, int max_count) {
    char query[512];
    int count = 0;

    if (!manager || !start_date || !end_date || !accounts || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, reg_date FROM member_white_account "
             "WHERE reg_date >= '%s' AND reg_date <= '%s' "
             "ORDER BY reg_date DESC LIMIT %d",
             start_date, end_date, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberWhiteAccount_GetRecentAdded(DBConnectionManager* manager, MemberWhiteAccount* accounts, int limit) {
    char query[512];
    int count = 0;

    if (!manager || !accounts || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, reg_date FROM member_white_account ORDER BY reg_date DESC LIMIT %d",
             limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

bool MemberWhiteAccount_IsWhitelisted(DBConnectionManager* manager, unsigned int m_id) {
    /* IsWhitelisted 和 Exists 功能相同，直接调用 */
    return MemberWhiteAccount_Exists(manager, m_id);
}

int MemberWhiteAccount_GetOldRecords(DBConnectionManager* manager, int days_ago,
                                      MemberWhiteAccount* accounts, int max_count) {
    char query[512];
    int count = 0;

    if (!manager || !accounts || max_count <= 0 || days_ago < 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, reg_date FROM member_white_account "
             "WHERE reg_date < DATE_SUB(NOW(), INTERVAL %d DAY) "
             "ORDER BY reg_date ASC LIMIT %d",
             days_ago, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillAccountFromRow(values, &accounts[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberWhiteAccount_DeleteByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date) {
    char query[512];

    if (!manager || !start_date || !end_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM member_white_account WHERE reg_date >= '%s' AND reg_date <= '%s'",
             start_date, end_date);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int affected_rows = ret;
    DBQueryResult_Free(&result);
    return affected_rows;
}

/* ========================================================================
 * 批量操作
 * ======================================================================== */

int MemberWhiteAccount_AddBatch(DBConnectionManager* manager, MemberWhiteAccount* accounts, int count) {
    int i;
    int success_count = 0;

    if (!manager || !accounts || count <= 0) {
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (MemberWhiteAccount_Add(manager, &accounts[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberWhiteAccount_DeleteBatch(DBConnectionManager* manager, unsigned int* m_ids, int count) {
    int i;
    int success_count = 0;

    if (!manager || !m_ids || count <= 0) {
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (MemberWhiteAccount_Delete(manager, m_ids[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

/* ========================================================================
 * 工具函数
 * ======================================================================== */

void MemberWhiteAccount_Print(const MemberWhiteAccount* account) {
    if (!account) {
        printf("[MemberWhiteAccount] NULL pointer\n");
        return;
    }

    printf("\n========== Member White Account ==========\n");
    printf("  Member ID: %u\n", account->m_id);
    printf("  Registration Date: %s\n", account->reg_date);
    printf("==========================================\n\n");
}
