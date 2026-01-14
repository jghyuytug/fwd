#include "member_doubt_trade_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 2048

int MemberDoubtTrade_Add(DBConnectionManager* manager, const MemberDoubtTrade* trade) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !trade) return -1;

    if (trade->last_update_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO member_doubt_trade (m_id, last_update_time, over_count) "
            "VALUES (%d, '%s', %u)", trade->m_id, trade->last_update_time, trade->over_count);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO member_doubt_trade (m_id, last_update_time, over_count) "
            "VALUES (%d, NOW(), %u)", trade->m_id, trade->over_count);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDoubtTrade_Get(DBConnectionManager* manager, int m_id, MemberDoubtTrade* trade) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !trade) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, last_update_time, over_count FROM member_doubt_trade WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(trade, 0, sizeof(MemberDoubtTrade));
    trade->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(trade->last_update_time, row[1], sizeof(trade->last_update_time) - 1);
    trade->over_count = row[2] ? (unsigned short)atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDoubtTrade_Update(DBConnectionManager* manager, const MemberDoubtTrade* trade) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !trade) return -1;

    if (trade->last_update_time[0]) {
        snprintf(query, sizeof(query),
            "UPDATE member_doubt_trade SET last_update_time = '%s', over_count = %u WHERE m_id = %d",
            trade->last_update_time, trade->over_count, trade->m_id);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE member_doubt_trade SET last_update_time = NOW(), over_count = %u WHERE m_id = %d",
            trade->over_count, trade->m_id);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDoubtTrade_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_doubt_trade WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDoubtTrade_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_doubt_trade WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int MemberDoubtTrade_IncrementCount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_doubt_trade SET over_count = over_count + 1, "
        "last_update_time = NOW() WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDoubtTrade_GetByMinCount(DBConnectionManager* manager, unsigned short min_count,
                                    MemberDoubtTrade* trades, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !trades || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, last_update_time, over_count FROM member_doubt_trade "
        "WHERE over_count >= %u ORDER BY over_count DESC LIMIT %d", min_count, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&trades[count], 0, sizeof(MemberDoubtTrade));
        trades[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(trades[count].last_update_time, row[1],
                           sizeof(trades[count].last_update_time) - 1);
        trades[count].over_count = row[2] ? (unsigned short)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberDoubtTrade_DeleteAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM member_doubt_trade");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void MemberDoubtTrade_PrintInfo(const MemberDoubtTrade* trade) {
    if (!trade) {
        printf("MemberDoubtTrade: NULL\n");
        return;
    }

    printf("MemberDoubtTrade:\n");
    printf("  m_id: %d\n", trade->m_id);
    printf("  last_update_time: %s\n", trade->last_update_time);
    printf("  over_count: %u\n", trade->over_count);
}
