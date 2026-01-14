#include "event_ingame_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventIngameHistory_Add(DBConnectionManager* manager, const EventIngameHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_ingame_history (m_id, type, history_1, history_2) "
        "VALUES (%d, %d, %d, %d)",
        record->m_id, record->type, record->history_1, record->history_2);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventIngameHistory_Get(DBConnectionManager* manager, int m_id, int type, EventIngameHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, type, history_1, history_2 FROM event_ingame_history WHERE m_id = %d AND type = %d",
        m_id, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventIngameHistory));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->type = row[1] ? atoi(row[1]) : 0;
    record->history_1 = row[2] ? atoi(row[2]) : 0;
    record->history_2 = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventIngameHistory_GetAll(DBConnectionManager* manager, EventIngameHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, type, history_1, history_2 FROM event_ingame_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventIngameHistory));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].type = row[1] ? atoi(row[1]) : 0;
        records[count].history_1 = row[2] ? atoi(row[2]) : 0;
        records[count].history_2 = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
