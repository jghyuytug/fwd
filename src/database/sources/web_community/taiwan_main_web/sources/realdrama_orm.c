#include "realdrama_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Realdrama_Add(DBConnectionManager* manager, const Realdrama* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO realdrama (title, create_day, broadcast_day, appear, plan, play, summary, content, view, banner1, banner2, file_url, open_flag, main_flag) "
        "VALUES ('%s', %d, %d, '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', %d, %d)",
        record->title, record->create_day, record->broadcast_day, record->appear, record->plan, record->play, record->summary, record->content, record->view, record->banner1, record->banner2, record->file_url, record->open_flag, record->main_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Realdrama_Get(DBConnectionManager* manager, int no, Realdrama* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, create_day, broadcast_day, appear, plan, play, summary, content, view, banner1, banner2, file_url, open_flag, main_flag FROM realdrama WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Realdrama));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    record->create_day = row[2] ? atoi(row[2]) : 0;
    record->broadcast_day = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->appear, row[4], sizeof(record->appear) - 1);
    if (row[5]) strncpy(record->plan, row[5], sizeof(record->plan) - 1);
    if (row[6]) strncpy(record->play, row[6], sizeof(record->play) - 1);
    if (row[7]) strncpy(record->summary, row[7], sizeof(record->summary) - 1);
    if (row[8]) strncpy(record->content, row[8], sizeof(record->content) - 1);
    record->view = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->banner1, row[10], sizeof(record->banner1) - 1);
    if (row[11]) strncpy(record->banner2, row[11], sizeof(record->banner2) - 1);
    if (row[12]) strncpy(record->file_url, row[12], sizeof(record->file_url) - 1);
    record->open_flag = row[13] ? atoi(row[13]) : 0;
    record->main_flag = row[14] ? atoi(row[14]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Realdrama_GetAll(DBConnectionManager* manager, Realdrama* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, create_day, broadcast_day, appear, plan, play, summary, content, view, banner1, banner2, file_url, open_flag, main_flag FROM realdrama");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Realdrama));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        records[count].create_day = row[2] ? atoi(row[2]) : 0;
        records[count].broadcast_day = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].appear, row[4], sizeof(records[count].appear) - 1);
        if (row[5]) strncpy(records[count].plan, row[5], sizeof(records[count].plan) - 1);
        if (row[6]) strncpy(records[count].play, row[6], sizeof(records[count].play) - 1);
        if (row[7]) strncpy(records[count].summary, row[7], sizeof(records[count].summary) - 1);
        if (row[8]) strncpy(records[count].content, row[8], sizeof(records[count].content) - 1);
        records[count].view = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].banner1, row[10], sizeof(records[count].banner1) - 1);
        if (row[11]) strncpy(records[count].banner2, row[11], sizeof(records[count].banner2) - 1);
        if (row[12]) strncpy(records[count].file_url, row[12], sizeof(records[count].file_url) - 1);
        records[count].open_flag = row[13] ? atoi(row[13]) : 0;
        records[count].main_flag = row[14] ? atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
