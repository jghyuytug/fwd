#include "max_count_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MaxCountChannel_Add(DBConnectionManager* manager, const MaxCountChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO max_count_channel (server_info, gc_channeltype, mc_max, mc_date) "
        "VALUES (%d, '%s', %d, '%s')",
        record->server_info, record->gc_channeltype, record->mc_max, record->mc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_GetAll(DBConnectionManager* manager, MaxCountChannel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date FROM max_count_channel");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountChannel));
        records[count].server_info = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].gc_channeltype, row[1], sizeof(records[count].gc_channeltype) - 1);
        records[count].mc_max = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].mc_date, row[3], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
