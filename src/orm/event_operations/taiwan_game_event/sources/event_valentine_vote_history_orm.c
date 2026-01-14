#include "event_valentine_vote_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventValentineVoteHistory_Add(DBConnectionManager* manager, const EventValentineVoteHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_valentine_vote_history (check_date, server_id, vote_A, vote_B, vote_C) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->check_date, record->server_id, record->vote_A, record->vote_B, record->vote_C);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventValentineVoteHistory_Get(DBConnectionManager* manager, const char* check_date, signed char server_id, EventValentineVoteHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT check_date, server_id, vote_A, vote_B, vote_C FROM event_valentine_vote_history WHERE check_date = '%s' AND server_id = %d",
        check_date, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventValentineVoteHistory));
    if (row[0]) strncpy(record->check_date, row[0], sizeof(record->check_date) - 1);
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->vote_A = row[2] ? atoi(row[2]) : 0;
    record->vote_B = row[3] ? atoi(row[3]) : 0;
    record->vote_C = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventValentineVoteHistory_GetAll(DBConnectionManager* manager, EventValentineVoteHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT check_date, server_id, vote_A, vote_B, vote_C FROM event_valentine_vote_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventValentineVoteHistory));
        if (row[0]) strncpy(records[count].check_date, row[0], sizeof(records[count].check_date) - 1);
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].vote_A = row[2] ? atoi(row[2]) : 0;
        records[count].vote_B = row[3] ? atoi(row[3]) : 0;
        records[count].vote_C = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
