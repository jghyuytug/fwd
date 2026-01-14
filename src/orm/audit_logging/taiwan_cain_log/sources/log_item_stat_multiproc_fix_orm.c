#include "log_item_stat_multiproc_fix_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogItemStatMultiprocFix_Add(DBConnectionManager* manager, const LogItemStatMultiprocFix* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_item_stat_multiproc_fix (cur_date, server_info, process_id, item_id, item_cnt) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->cur_date, record->server_info, record->process_id, record->item_id, record->item_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogItemStatMultiprocFix_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemStatMultiprocFix* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, server_info, process_id, item_id, item_cnt FROM log_item_stat_multiproc_fix WHERE cur_date = '%s' AND server_info = %d AND process_id = %d AND item_id = %d",
        cur_date, server_info, process_id, item_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogItemStatMultiprocFix));
    if (row[0]) strncpy(record->cur_date, row[0], sizeof(record->cur_date) - 1);
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->process_id = row[2] ? atoi(row[2]) : 0;
    record->item_id = row[3] ? atoi(row[3]) : 0;
    record->item_cnt = row[4] ? atoll(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogItemStatMultiprocFix_GetAll(DBConnectionManager* manager, LogItemStatMultiprocFix* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, server_info, process_id, item_id, item_cnt FROM log_item_stat_multiproc_fix");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogItemStatMultiprocFix));
        if (row[0]) strncpy(records[count].cur_date, row[0], sizeof(records[count].cur_date) - 1);
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].process_id = row[2] ? atoi(row[2]) : 0;
        records[count].item_id = row[3] ? atoi(row[3]) : 0;
        records[count].item_cnt = row[4] ? atoll(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
