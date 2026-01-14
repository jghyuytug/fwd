#include "class_dbs_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ClassDbsLog_Add(DBConnectionManager* manager, const ClassDbsLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO class_dbs_log (memo, reg_date) "
        "VALUES ('%s', '%s')",
        record->memo, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ClassDbsLog_Get(DBConnectionManager* manager, int log_id, ClassDbsLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT log_id, memo, reg_date FROM class_dbs_log WHERE log_id = %d",
        log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ClassDbsLog));
    record->log_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->memo, row[1], sizeof(record->memo) - 1);
    if (row[2]) strncpy(record->reg_date, row[2], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ClassDbsLog_GetAll(DBConnectionManager* manager, ClassDbsLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT log_id, memo, reg_date FROM class_dbs_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ClassDbsLog));
        records[count].log_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].memo, row[1], sizeof(records[count].memo) - 1);
        if (row[2]) strncpy(records[count].reg_date, row[2], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
