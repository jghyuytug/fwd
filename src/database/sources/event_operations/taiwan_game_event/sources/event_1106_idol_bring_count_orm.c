#include "event_1106_idol_bring_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Event1106IdolBringCount_Add(DBConnectionManager* manager, const Event1106IdolBringCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_1106_idol_bring_count (server_id, pot_type, r_count, adjust_value) "
        "VALUES (%d, %d, %d, %d)",
        record->server_id, record->pot_type, record->r_count, record->adjust_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringCount_Get(DBConnectionManager* manager, signed char server_id, signed char pot_type, Event1106IdolBringCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, pot_type, r_count, adjust_value FROM event_1106_idol_bring_count WHERE server_id = %d AND pot_type = %d",
        server_id, pot_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Event1106IdolBringCount));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->pot_type = row[1] ? atoi(row[1]) : 0;
    record->r_count = row[2] ? atoi(row[2]) : 0;
    record->adjust_value = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringCount_GetAll(DBConnectionManager* manager, Event1106IdolBringCount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, pot_type, r_count, adjust_value FROM event_1106_idol_bring_count");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Event1106IdolBringCount));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].pot_type = row[1] ? atoi(row[1]) : 0;
        records[count].r_count = row[2] ? atoi(row[2]) : 0;
        records[count].adjust_value = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
