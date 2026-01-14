#include "power_war_statue_ranker_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PowerWarStatueRanker_Add(DBConnectionManager* manager, const PowerWarStatueRanker* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO power_war_statue_ranker (server_id, first_ranker, second_ranker, third_ranker) "
        "VALUES (%d, %d, %d, %d)",
        record->server_id, record->first_ranker, record->second_ranker, record->third_ranker);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWarStatueRanker_Get(DBConnectionManager* manager, signed char server_id, PowerWarStatueRanker* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, first_ranker, second_ranker, third_ranker FROM power_war_statue_ranker WHERE server_id = %d",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PowerWarStatueRanker));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->first_ranker = row[1] ? atoi(row[1]) : 0;
    record->second_ranker = row[2] ? atoi(row[2]) : 0;
    record->third_ranker = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWarStatueRanker_GetAll(DBConnectionManager* manager, PowerWarStatueRanker* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, first_ranker, second_ranker, third_ranker FROM power_war_statue_ranker");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PowerWarStatueRanker));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].first_ranker = row[1] ? atoi(row[1]) : 0;
        records[count].second_ranker = row[2] ? atoi(row[2]) : 0;
        records[count].third_ranker = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
