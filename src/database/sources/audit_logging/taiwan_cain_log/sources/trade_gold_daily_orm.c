#include "trade_gold_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TradeGoldDaily_Add(DBConnectionManager* manager, const TradeGoldDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO trade_gold_daily (m_id, gold, occ_time) "
        "VALUES (%d, %d, '%s')",
        record->m_id, record->gold, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TradeGoldDaily_Get(DBConnectionManager* manager, int m_id, TradeGoldDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, gold, occ_time FROM trade_gold_daily WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TradeGoldDaily));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->gold = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int TradeGoldDaily_GetAll(DBConnectionManager* manager, TradeGoldDaily* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, gold, occ_time FROM trade_gold_daily");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TradeGoldDaily));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].gold = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
