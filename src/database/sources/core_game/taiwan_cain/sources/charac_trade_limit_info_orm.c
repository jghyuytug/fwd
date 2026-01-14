#include "charac_trade_limit_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作 ==================== */

int CharacTradeLimitInfo_Add(DBConnectionManager* manager, const CharacTradeLimitInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_trade_limit_info "
        "(m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user) "
        "VALUES (%u, %u, '%s', %u, %u, %u)",
        record->m_id, record->charac_no, record->last_trade_time,
        record->total_trade_gold, record->trade_count, record->nexon_user);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacTradeLimitInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTradeLimitInfo));

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM charac_trade_limit_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = (unsigned int)atoi(values[0]);
        record->charac_no = (unsigned int)atoi(values[1]);
        if (values[2]) strncpy(record->last_trade_time, values[2], sizeof(record->last_trade_time) - 1);
        record->total_trade_gold = (unsigned int)atoi(values[3]);
        record->trade_count = (unsigned short)atoi(values[4]);
        record->nexon_user = (unsigned char)atoi(values[5]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTradeLimitInfo_Update(DBConnectionManager* manager, const CharacTradeLimitInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET "
        "m_id = %u, last_trade_time = '%s', total_trade_gold = %u, trade_count = %u, nexon_user = %u "
        "WHERE charac_no = %u",
        record->m_id, record->last_trade_time, record->total_trade_gold,
        record->trade_count, record->nexon_user, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_trade_limit_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_trade_limit_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 专项操作 ==================== */

int CharacTradeLimitInfo_UpdateLastTradeTime(DBConnectionManager* manager, unsigned int charac_no, const char* trade_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !trade_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET last_trade_time = '%s' WHERE charac_no = %u",
        trade_time, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_IncrementTradeGold(DBConnectionManager* manager, unsigned int charac_no, unsigned int gold) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET total_trade_gold = total_trade_gold + %u, last_trade_time = NOW() "
        "WHERE charac_no = %u",
        gold, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_IncrementTradeCount(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET trade_count = trade_count + 1, last_trade_time = NOW() "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_ResetTradeStats(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET total_trade_gold = 0, trade_count = 0 WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTradeLimitInfo_UpdateNexonUser(DBConnectionManager* manager, unsigned int charac_no, unsigned char nexon_user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET nexon_user = %u WHERE charac_no = %u",
        nexon_user, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int CharacTradeLimitInfo_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                       CharacTradeLimitInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM charac_trade_limit_info WHERE m_id = %u ORDER BY total_trade_gold DESC LIMIT %d",
        m_id, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].charac_no = (unsigned int)atoi(values[1]);
        if (values[2]) strncpy(records[count].last_trade_time, values[2], sizeof(records[count].last_trade_time) - 1);
        records[count].total_trade_gold = (unsigned int)atoi(values[3]);
        records[count].trade_count = (unsigned short)atoi(values[4]);
        records[count].nexon_user = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTradeLimitInfo_GetByMinGold(DBConnectionManager* manager, unsigned int min_gold,
                                      CharacTradeLimitInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM charac_trade_limit_info WHERE total_trade_gold >= %u ORDER BY total_trade_gold DESC LIMIT %d",
        min_gold, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].charac_no = (unsigned int)atoi(values[1]);
        if (values[2]) strncpy(records[count].last_trade_time, values[2], sizeof(records[count].last_trade_time) - 1);
        records[count].total_trade_gold = (unsigned int)atoi(values[3]);
        records[count].trade_count = (unsigned short)atoi(values[4]);
        records[count].nexon_user = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTradeLimitInfo_GetByMinTradeCount(DBConnectionManager* manager, unsigned short min_count,
                                             CharacTradeLimitInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM charac_trade_limit_info WHERE trade_count >= %u ORDER BY trade_count DESC LIMIT %d",
        min_count, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].charac_no = (unsigned int)atoi(values[1]);
        if (values[2]) strncpy(records[count].last_trade_time, values[2], sizeof(records[count].last_trade_time) - 1);
        records[count].total_trade_gold = (unsigned int)atoi(values[3]);
        records[count].trade_count = (unsigned short)atoi(values[4]);
        records[count].nexon_user = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTradeLimitInfo_GetByTradeTimeAfter(DBConnectionManager* manager, const char* after_time,
                                              CharacTradeLimitInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !after_time || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM charac_trade_limit_info WHERE last_trade_time > '%s' ORDER BY last_trade_time DESC LIMIT %d",
        after_time, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].charac_no = (unsigned int)atoi(values[1]);
        if (values[2]) strncpy(records[count].last_trade_time, values[2], sizeof(records[count].last_trade_time) - 1);
        records[count].total_trade_gold = (unsigned int)atoi(values[3]);
        records[count].trade_count = (unsigned short)atoi(values[4]);
        records[count].nexon_user = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTradeLimitInfo_GetNexonUsers(DBConnectionManager* manager, CharacTradeLimitInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM charac_trade_limit_info WHERE nexon_user = 1 ORDER BY total_trade_gold DESC LIMIT %d",
        max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].charac_no = (unsigned int)atoi(values[1]);
        if (values[2]) strncpy(records[count].last_trade_time, values[2], sizeof(records[count].last_trade_time) - 1);
        records[count].total_trade_gold = (unsigned int)atoi(values[3]);
        records[count].trade_count = (unsigned short)atoi(values[4]);
        records[count].nexon_user = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTradeLimitInfo_BatchResetTradeStats(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_trade_limit_info SET total_trade_gold = 0, trade_count = 0");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数 ==================== */

void CharacTradeLimitInfo_PrintInfo(const CharacTradeLimitInfo* record) {
    if (!record) {
        printf("CharacTradeLimitInfo: NULL\n");
        return;
    }

    printf("\nCharacTradeLimitInfo {\n");
    printf("  m_id: %u\n", record->m_id);
    printf("  charac_no: %u\n", record->charac_no);
    printf("  last_trade_time: %s\n", record->last_trade_time);
    printf("  total_trade_gold: %u\n", record->total_trade_gold);
    printf("  trade_count: %u\n", record->trade_count);
    printf("  nexon_user: %u\n", record->nexon_user);
    printf("}\n");
}
