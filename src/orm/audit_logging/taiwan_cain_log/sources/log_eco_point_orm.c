#include "log_eco_point_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogEcoPoint_Add(DBConnectionManager* manager, const LogEcoPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_eco_point (charac_no, save_time, item_id, item_cnt, add_point, use_point, save_type) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d)",
        record->charac_no, record->save_time, record->item_id, record->item_cnt, record->add_point, record->use_point, record->save_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogEcoPoint_Get(DBConnectionManager* manager, int charac_no, int save_time, LogEcoPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, save_time, item_id, item_cnt, add_point, use_point, save_type FROM log_eco_point WHERE charac_no = %d AND save_time = %d",
        charac_no, save_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogEcoPoint));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->save_time = row[1] ? atoi(row[1]) : 0;
    record->item_id = row[2] ? atoi(row[2]) : 0;
    record->item_cnt = row[3] ? atoi(row[3]) : 0;
    record->add_point = row[4] ? atoi(row[4]) : 0;
    record->use_point = row[5] ? atoi(row[5]) : 0;
    record->save_type = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogEcoPoint_GetAll(DBConnectionManager* manager, LogEcoPoint* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, save_time, item_id, item_cnt, add_point, use_point, save_type FROM log_eco_point");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogEcoPoint));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].save_time = row[1] ? atoi(row[1]) : 0;
        records[count].item_id = row[2] ? atoi(row[2]) : 0;
        records[count].item_cnt = row[3] ? atoi(row[3]) : 0;
        records[count].add_point = row[4] ? atoi(row[4]) : 0;
        records[count].use_point = row[5] ? atoi(row[5]) : 0;
        records[count].save_type = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
