#include "event_valentine_vote_base_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventValentineVoteBase_Add(DBConnectionManager* manager, const EventValentineVoteBase* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_valentine_vote_base (server_id, stage1, stage2, stage3, stage4, stage5, cur_event) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d)",
        record->server_id, record->stage1, record->stage2, record->stage3, record->stage4, record->stage5, record->cur_event);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventValentineVoteBase_Get(DBConnectionManager* manager, signed char server_id, EventValentineVoteBase* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, stage1, stage2, stage3, stage4, stage5, cur_event FROM event_valentine_vote_base WHERE server_id = %d",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventValentineVoteBase));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->stage1 = row[1] ? atoi(row[1]) : 0;
    record->stage2 = row[2] ? atoi(row[2]) : 0;
    record->stage3 = row[3] ? atoi(row[3]) : 0;
    record->stage4 = row[4] ? atoi(row[4]) : 0;
    record->stage5 = row[5] ? atoi(row[5]) : 0;
    record->cur_event = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventValentineVoteBase_GetAll(DBConnectionManager* manager, EventValentineVoteBase* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, stage1, stage2, stage3, stage4, stage5, cur_event FROM event_valentine_vote_base");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventValentineVoteBase));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].stage1 = row[1] ? atoi(row[1]) : 0;
        records[count].stage2 = row[2] ? atoi(row[2]) : 0;
        records[count].stage3 = row[3] ? atoi(row[3]) : 0;
        records[count].stage4 = row[4] ? atoi(row[4]) : 0;
        records[count].stage5 = row[5] ? atoi(row[5]) : 0;
        records[count].cur_event = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
