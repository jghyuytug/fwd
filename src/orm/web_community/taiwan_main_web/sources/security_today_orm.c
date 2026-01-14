#include "security_today_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SecurityToday_Add(DBConnectionManager* manager, const SecurityToday* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO security_today (day, contents, link, window_flag) "
        "VALUES ('%s', '%s', '%s', %d)",
        record->day, record->contents, record->link, record->window_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SecurityToday_Get(DBConnectionManager* manager, int no, SecurityToday* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, day, contents, link, window_flag FROM security_today WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SecurityToday));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->day, row[1], sizeof(record->day) - 1);
    if (row[2]) strncpy(record->contents, row[2], sizeof(record->contents) - 1);
    if (row[3]) strncpy(record->link, row[3], sizeof(record->link) - 1);
    record->window_flag = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SecurityToday_GetAll(DBConnectionManager* manager, SecurityToday* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, day, contents, link, window_flag FROM security_today");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SecurityToday));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].day, row[1], sizeof(records[count].day) - 1);
        if (row[2]) strncpy(records[count].contents, row[2], sizeof(records[count].contents) - 1);
        if (row[3]) strncpy(records[count].link, row[3], sizeof(records[count].link) - 1);
        records[count].window_flag = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
