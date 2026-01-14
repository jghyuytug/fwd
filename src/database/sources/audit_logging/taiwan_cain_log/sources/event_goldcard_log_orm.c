#include "event_goldcard_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventGoldcardLog_Add(DBConnectionManager* manager, const EventGoldcardLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_goldcard_log (m_id, occ_date, cnt) "
        "VALUES (%d, '%s', %d)",
        record->m_id, record->occ_date, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardLog_Get(DBConnectionManager* manager, int m_id, const char* occ_date, EventGoldcardLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, cnt FROM event_goldcard_log WHERE m_id = %d AND occ_date = '%s'",
        m_id, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventGoldcardLog));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardLog_GetAll(DBConnectionManager* manager, EventGoldcardLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, cnt FROM event_goldcard_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventGoldcardLog));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
