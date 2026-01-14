#include "log_ontime_reward_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogOntimeReward_Add(DBConnectionManager* manager, const LogOntimeReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_ontime_reward (time, event_no, reward_cnt) "
        "VALUES ('%s', %d, %d)",
        record->time, record->event_no, record->reward_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogOntimeReward_Get(DBConnectionManager* manager, const char* time, int event_no, LogOntimeReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT time, event_no, reward_cnt FROM log_ontime_reward WHERE time = '%s' AND event_no = %d",
        time, event_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogOntimeReward));
    if (row[0]) strncpy(record->time, row[0], sizeof(record->time) - 1);
    record->event_no = row[1] ? atoi(row[1]) : 0;
    record->reward_cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogOntimeReward_GetAll(DBConnectionManager* manager, LogOntimeReward* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT time, event_no, reward_cnt FROM log_ontime_reward");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogOntimeReward));
        if (row[0]) strncpy(records[count].time, row[0], sizeof(records[count].time) - 1);
        records[count].event_no = row[1] ? atoi(row[1]) : 0;
        records[count].reward_cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
