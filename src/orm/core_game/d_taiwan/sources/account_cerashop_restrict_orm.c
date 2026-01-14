#include "account_cerashop_restrict_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作实现 ==================== */

int AccountCerashopRestrict_Add(DBConnectionManager* manager, const AccountCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "[AccountCerashopRestrict_Add] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO account_cerashop_restrict "
        "(m_id, ipg_no, count, next_date, end_date, last_access_date) "
        "VALUES (%u, %u, %u, %u, %u, %u)",
        record->m_id,
        record->ipg_no,
        record->count,
        record->next_date,
        record->end_date,
        record->last_access_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_Add] Query failed: %s\n", query);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_Get(DBConnectionManager* manager, unsigned int m_id,
                                unsigned int ipg_no, AccountCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int field_count;

    if (!manager || !record) {
        fprintf(stderr, "[AccountCerashopRestrict_Get] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, ipg_no, count, next_date, end_date, last_access_date "
        "FROM account_cerashop_restrict "
        "WHERE m_id = %u AND ipg_no = %u",
        m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_Get] Query failed\n");
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    field_count = DBQueryResult_FetchRow(&result, fields);
    if (field_count < 6) {
        fprintf(stderr, "[AccountCerashopRestrict_Get] Incomplete data\n");
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析字段 */
    record->m_id = fields[0] ? (unsigned int)atoi(fields[0]) : 0;
    record->ipg_no = fields[1] ? (unsigned int)atoi(fields[1]) : 0;
    record->count = fields[2] ? (unsigned int)atoi(fields[2]) : 0;
    record->next_date = fields[3] ? (unsigned int)atoi(fields[3]) : 0;
    record->end_date = fields[4] ? (unsigned int)atoi(fields[4]) : 0;
    record->last_access_date = fields[5] ? (unsigned int)atoi(fields[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_Update(DBConnectionManager* manager, const AccountCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "[AccountCerashopRestrict_Update] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE account_cerashop_restrict "
        "SET count = %u, next_date = %u, end_date = %u, last_access_date = %u "
        "WHERE m_id = %u AND ipg_no = %u",
        record->count,
        record->next_date,
        record->end_date,
        record->last_access_date,
        record->m_id,
        record->ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_Update] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_Delete] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM account_cerashop_restrict WHERE m_id = %u AND ipg_no = %u",
        m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_Delete] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int exists;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_Exists] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM account_cerashop_restrict WHERE m_id = %u AND ipg_no = %u LIMIT 1",
        m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_Exists] Query failed\n");
        return -1;
    }

    exists = (result.row_count > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 业务操作实现 ==================== */

int AccountCerashopRestrict_IncrementCount(DBConnectionManager* manager, unsigned int m_id,
                                           unsigned int ipg_no, unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_IncrementCount] Invalid manager\n");
        return -1;
    }

    if (increment == 0) {
        increment = 1; /* 默认增加1 */
    }

    snprintf(query, sizeof(query),
        "UPDATE account_cerashop_restrict "
        "SET count = count + %u "
        "WHERE m_id = %u AND ipg_no = %u",
        increment, m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_IncrementCount] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_UpdateLastAccess(DBConnectionManager* manager, unsigned int m_id,
                                             unsigned int ipg_no, unsigned int timestamp) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_UpdateLastAccess] Invalid manager\n");
        return -1;
    }

    if (timestamp == 0) {
        timestamp = (unsigned int)time(NULL); /* 使用当前时间 */
    }

    snprintf(query, sizeof(query),
        "UPDATE account_cerashop_restrict "
        "SET last_access_date = %u "
        "WHERE m_id = %u AND ipg_no = %u",
        timestamp, m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_UpdateLastAccess] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_CanPurchase(DBConnectionManager* manager, unsigned int m_id,
                                        unsigned int ipg_no, unsigned int current_time) {
    AccountCerashopRestrict record;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_CanPurchase] Invalid manager\n");
        return -1;
    }

    if (current_time == 0) {
        current_time = (unsigned int)time(NULL);
    }

    /* 尝试获取限制记录 */
    if (AccountCerashopRestrict_Get(manager, m_id, ipg_no, &record) < 0) {
        /* 没有限制记录，允许购买 */
        return 1;
    }

    /* 检查是否超过结束日期 */
    if (current_time > record.end_date) {
        /* 限制已过期，允许购买 */
        return 1;
    }

    /* 检查是否还在冷却期 */
    if (current_time < record.next_date) {
        /* 还在冷却期，禁止购买 */
        return 0;
    }

    /* 允许购买 */
    return 1;
}

int AccountCerashopRestrict_ResetCount(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_ResetCount] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE account_cerashop_restrict "
        "SET count = 0 "
        "WHERE m_id = %u AND ipg_no = %u",
        m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_ResetCount] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_GetCountByAccount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[AccountCerashopRestrict_GetCountByAccount] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM account_cerashop_restrict WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[AccountCerashopRestrict_GetCountByAccount] Query failed\n");
        return -1;
    }

    if (result.row_count == 0 || DBQueryResult_FetchRow(&result, fields) < 1) {
        DBQueryResult_Free(&result);
        return 0;
    }

    count = fields[0] ? atoi(fields[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void AccountCerashopRestrict_PrintInfo(const AccountCerashopRestrict* record) {
    if (!record) {
        printf("AccountCerashopRestrict: NULL\n");
        return;
    }

    printf("========================================\n");
    printf("Account Cerashop Restrict Information:\n");
    printf("========================================\n");
    printf("Account ID (m_id):     %u\n", record->m_id);
    printf("IPG Number (ipg_no):   %u\n", record->ipg_no);
    printf("Purchase Count:        %u\n", record->count);
    printf("Next Purchase Date:    %u\n", record->next_date);
    printf("Restriction End Date:  %u\n", record->end_date);
    printf("Last Access Date:      %u\n", record->last_access_date);
    printf("========================================\n");
}
