#include "realradio_again_0805_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int RealradioAgain0805_Add(DBConnectionManager* manager, const RealradioAgain0805* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO realradio_again_0805 (title, create_day, broadcast_day, file_url, useyn, view, play_num, scenario, read_cnt, search, comment_cnt) "
        "VALUES ('%s', %d, %d, '%s', %d, %d, %d, '%s', %d, '%s', %d)",
        record->title, record->create_day, record->broadcast_day, record->file_url, record->useyn, record->view, record->play_num, record->scenario, record->read_cnt, record->search, record->comment_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioAgain0805_Get(DBConnectionManager* manager, int no, RealradioAgain0805* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, create_day, broadcast_day, file_url, useyn, view, play_num, scenario, read_cnt, search, comment_cnt FROM realradio_again_0805 WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(RealradioAgain0805));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    record->create_day = row[2] ? atoi(row[2]) : 0;
    record->broadcast_day = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->file_url, row[4], sizeof(record->file_url) - 1);
    record->useyn = row[5] ? atoi(row[5]) : 0;
    record->view = row[6] ? atoi(row[6]) : 0;
    record->play_num = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->scenario, row[8], sizeof(record->scenario) - 1);
    record->read_cnt = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->search, row[10], sizeof(record->search) - 1);
    record->comment_cnt = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioAgain0805_GetAll(DBConnectionManager* manager, RealradioAgain0805* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, create_day, broadcast_day, file_url, useyn, view, play_num, scenario, read_cnt, search, comment_cnt FROM realradio_again_0805");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(RealradioAgain0805));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        records[count].create_day = row[2] ? atoi(row[2]) : 0;
        records[count].broadcast_day = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].file_url, row[4], sizeof(records[count].file_url) - 1);
        records[count].useyn = row[5] ? atoi(row[5]) : 0;
        records[count].view = row[6] ? atoi(row[6]) : 0;
        records[count].play_num = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].scenario, row[8], sizeof(records[count].scenario) - 1);
        records[count].read_cnt = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].search, row[10], sizeof(records[count].search) - 1);
        records[count].comment_cnt = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
