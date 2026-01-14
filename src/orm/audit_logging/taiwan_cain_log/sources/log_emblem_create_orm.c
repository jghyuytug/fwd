#include "log_emblem_create_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogEmblemCreate_Add(DBConnectionManager* manager, const LogEmblemCreate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_emblem_create (cur_date, grade0, grade1, grade2, grade3, grade4, grade5, grade6) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d)",
        record->cur_date, record->grade0, record->grade1, record->grade2, record->grade3, record->grade4, record->grade5, record->grade6);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogEmblemCreate_Get(DBConnectionManager* manager, const char* cur_date, LogEmblemCreate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, grade0, grade1, grade2, grade3, grade4, grade5, grade6 FROM log_emblem_create WHERE cur_date = '%s'",
        cur_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogEmblemCreate));
    if (row[0]) strncpy(record->cur_date, row[0], sizeof(record->cur_date) - 1);
    record->grade0 = row[1] ? atoi(row[1]) : 0;
    record->grade1 = row[2] ? atoi(row[2]) : 0;
    record->grade2 = row[3] ? atoi(row[3]) : 0;
    record->grade3 = row[4] ? atoi(row[4]) : 0;
    record->grade4 = row[5] ? atoi(row[5]) : 0;
    record->grade5 = row[6] ? atoi(row[6]) : 0;
    record->grade6 = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogEmblemCreate_GetAll(DBConnectionManager* manager, LogEmblemCreate* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, grade0, grade1, grade2, grade3, grade4, grade5, grade6 FROM log_emblem_create");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogEmblemCreate));
        if (row[0]) strncpy(records[count].cur_date, row[0], sizeof(records[count].cur_date) - 1);
        records[count].grade0 = row[1] ? atoi(row[1]) : 0;
        records[count].grade1 = row[2] ? atoi(row[2]) : 0;
        records[count].grade2 = row[3] ? atoi(row[3]) : 0;
        records[count].grade3 = row[4] ? atoi(row[4]) : 0;
        records[count].grade4 = row[5] ? atoi(row[5]) : 0;
        records[count].grade5 = row[6] ? atoi(row[6]) : 0;
        records[count].grade6 = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
