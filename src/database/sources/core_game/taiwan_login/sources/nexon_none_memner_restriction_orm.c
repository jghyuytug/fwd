#include "nexon_none_memner_restriction_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int NexonNoneMemnerRestriction_Add(DBConnectionManager* manager, const NexonNoneMemnerRestriction* restriction) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !restriction) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO nexon_none_memner_restriction "
        "(m_id, charac_id, last_trade_time, total_trade_gold, trade_count, nexon_user) "
        "VALUES (%u, %u, '%s', %u, %u, %d)",
        restriction->m_id, restriction->charac_id, restriction->last_trade_time,
        restriction->total_trade_gold, restriction->trade_count, restriction->nexon_user);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_Get(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_id,
                                    NexonNoneMemnerRestriction* restriction) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !restriction) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_id, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM nexon_none_memner_restriction WHERE m_id = %u AND charac_id = %u",
        m_id, charac_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(restriction, 0, sizeof(NexonNoneMemnerRestriction));
    restriction->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    restriction->charac_id = row[1] ? (unsigned int)atoi(row[1]) : 0;
    if (row[2]) strncpy(restriction->last_trade_time, row[2], sizeof(restriction->last_trade_time) - 1);
    restriction->total_trade_gold = row[3] ? (unsigned int)atoi(row[3]) : 0;
    restriction->trade_count = row[4] ? (unsigned short)atoi(row[4]) : 0;
    restriction->nexon_user = row[5] ? (signed char)atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_Update(DBConnectionManager* manager, const NexonNoneMemnerRestriction* restriction) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !restriction) return -1;

    snprintf(query, sizeof(query),
        "UPDATE nexon_none_memner_restriction SET "
        "last_trade_time = '%s', total_trade_gold = %u, trade_count = %u, nexon_user = %d "
        "WHERE m_id = %u AND charac_id = %u",
        restriction->last_trade_time, restriction->total_trade_gold, restriction->trade_count,
        restriction->nexon_user, restriction->m_id, restriction->charac_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM nexon_none_memner_restriction WHERE m_id = %u AND charac_id = %u",
        m_id, charac_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM nexon_none_memner_restriction WHERE m_id = %u AND charac_id = %u LIMIT 1",
        m_id, charac_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    int exists = (DBQueryResult_FetchRow(&result, row) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

int NexonNoneMemnerRestriction_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                            NexonNoneMemnerRestriction* restrictions,
                                            int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !restrictions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_id, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM nexon_none_memner_restriction WHERE m_id = %u ORDER BY charac_id", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&restrictions[count], 0, sizeof(NexonNoneMemnerRestriction));
        restrictions[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        restrictions[count].charac_id = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) strncpy(restrictions[count].last_trade_time, row[2], sizeof(restrictions[count].last_trade_time) - 1);
        restrictions[count].total_trade_gold = row[3] ? (unsigned int)atoi(row[3]) : 0;
        restrictions[count].trade_count = row[4] ? (unsigned short)atoi(row[4]) : 0;
        restrictions[count].nexon_user = row[5] ? (signed char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_GetByNexonStatus(DBConnectionManager* manager, signed char nexon_user,
                                                 NexonNoneMemnerRestriction* restrictions,
                                                 int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !restrictions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_id, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM nexon_none_memner_restriction WHERE nexon_user = %d ORDER BY m_id", nexon_user);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&restrictions[count], 0, sizeof(NexonNoneMemnerRestriction));
        restrictions[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        restrictions[count].charac_id = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) strncpy(restrictions[count].last_trade_time, row[2], sizeof(restrictions[count].last_trade_time) - 1);
        restrictions[count].total_trade_gold = row[3] ? (unsigned int)atoi(row[3]) : 0;
        restrictions[count].trade_count = row[4] ? (unsigned short)atoi(row[4]) : 0;
        restrictions[count].nexon_user = row[5] ? (signed char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_UpdateTradeInfo(DBConnectionManager* manager, unsigned int m_id,
                                                unsigned int charac_id, unsigned int gold_amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE nexon_none_memner_restriction SET "
        "last_trade_time = NOW(), total_trade_gold = total_trade_gold + %u, trade_count = trade_count + 1 "
        "WHERE m_id = %u AND charac_id = %u",
        gold_amount, m_id, charac_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int NexonNoneMemnerRestriction_GetHighTraders(DBConnectionManager* manager, unsigned int min_gold,
                                               NexonNoneMemnerRestriction* restrictions,
                                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !restrictions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_id, last_trade_time, total_trade_gold, trade_count, nexon_user "
        "FROM nexon_none_memner_restriction WHERE total_trade_gold >= %u "
        "ORDER BY total_trade_gold DESC", min_gold);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&restrictions[count], 0, sizeof(NexonNoneMemnerRestriction));
        restrictions[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        restrictions[count].charac_id = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) strncpy(restrictions[count].last_trade_time, row[2], sizeof(restrictions[count].last_trade_time) - 1);
        restrictions[count].total_trade_gold = row[3] ? (unsigned int)atoi(row[3]) : 0;
        restrictions[count].trade_count = row[4] ? (unsigned short)atoi(row[4]) : 0;
        restrictions[count].nexon_user = row[5] ? (signed char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void NexonNoneMemnerRestriction_PrintInfo(const NexonNoneMemnerRestriction* restriction) {
    if (!restriction) return;

    printf("NexonNoneMemnerRestriction {\n");
    printf("  m_id: %u\n", restriction->m_id);
    printf("  charac_id: %u\n", restriction->charac_id);
    printf("  last_trade_time: %s\n", restriction->last_trade_time);
    printf("  total_trade_gold: %u\n", restriction->total_trade_gold);
    printf("  trade_count: %u\n", restriction->trade_count);
    printf("  nexon_user: %d\n", restriction->nexon_user);
    printf("}\n");
}
