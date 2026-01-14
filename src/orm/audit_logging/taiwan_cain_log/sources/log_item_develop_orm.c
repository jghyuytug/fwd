#include "log_item_develop_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogItemDevelop_Add(DBConnectionManager* manager, const LogItemDevelop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_item_develop (occ_time, it_no, server_id, charac_no, charac_name, develop, sucess_flag) "
        "VALUES ('%s', %d, %d, %d, '%s', %d, %d)",
        record->occ_time, record->it_no, record->server_id, record->charac_no, record->charac_name, record->develop, record->sucess_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogItemDevelop_Get(DBConnectionManager* manager, const char* occ_time, int it_no, LogItemDevelop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, it_no, server_id, charac_no, charac_name, develop, sucess_flag FROM log_item_develop WHERE occ_time = '%s' AND it_no = %d",
        occ_time, it_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogItemDevelop));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->it_no = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    record->develop = row[5] ? atoi(row[5]) : 0;
    record->sucess_flag = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogItemDevelop_GetAll(DBConnectionManager* manager, LogItemDevelop* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, it_no, server_id, charac_no, charac_name, develop, sucess_flag FROM log_item_develop");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogItemDevelop));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].it_no = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        records[count].develop = row[5] ? atoi(row[5]) : 0;
        records[count].sucess_flag = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
