#include "realradio_again_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int RealradioAgain_Add(DBConnectionManager* manager, const RealradioAgain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO realradio_again (title, create_day, broadcast_day, file_url, useyn, view) "
        "VALUES ('%s', %d, %d, '%s', %d, %d)",
        record->title, record->create_day, record->broadcast_day, record->file_url, record->useyn, record->view);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioAgain_Get(DBConnectionManager* manager, int no, RealradioAgain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, create_day, broadcast_day, file_url, useyn, view FROM realradio_again WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(RealradioAgain));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    record->create_day = row[2] ? atoi(row[2]) : 0;
    record->broadcast_day = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->file_url, row[4], sizeof(record->file_url) - 1);
    record->useyn = row[5] ? atoi(row[5]) : 0;
    record->view = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioAgain_GetAll(DBConnectionManager* manager, RealradioAgain* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, create_day, broadcast_day, file_url, useyn, view FROM realradio_again");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(RealradioAgain));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        records[count].create_day = row[2] ? atoi(row[2]) : 0;
        records[count].broadcast_day = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].file_url, row[4], sizeof(records[count].file_url) - 1);
        records[count].useyn = row[5] ? atoi(row[5]) : 0;
        records[count].view = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
