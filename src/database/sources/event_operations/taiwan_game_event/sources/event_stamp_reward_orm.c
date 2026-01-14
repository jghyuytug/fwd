#include "event_stamp_reward_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventStampReward_Add(DBConnectionManager* manager, const EventStampReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_stamp_reward (check_step, item_id, item_cnt) "
        "VALUES (%d, %d, %d)",
        record->check_step, record->item_id, record->item_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventStampReward_Get(DBConnectionManager* manager, short check_step, EventStampReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT check_step, item_id, item_cnt FROM event_stamp_reward WHERE check_step = %d",
        check_step);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventStampReward));
    record->check_step = row[0] ? atoi(row[0]) : 0;
    record->item_id = row[1] ? atoi(row[1]) : 0;
    record->item_cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventStampReward_GetAll(DBConnectionManager* manager, EventStampReward* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT check_step, item_id, item_cnt FROM event_stamp_reward");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventStampReward));
        records[count].check_step = row[0] ? atoi(row[0]) : 0;
        records[count].item_id = row[1] ? atoi(row[1]) : 0;
        records[count].item_cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
