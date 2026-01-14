#include "update_act_category_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UpdateActCategory_Add(DBConnectionManager* manager, const UpdateActCategory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO update_act_category (act_no, category_no, order_no, subject, content, img_url) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s')",
        record->act_no, record->category_no, record->order_no, record->subject, record->content, record->img_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UpdateActCategory_Get(DBConnectionManager* manager, int act_no, signed char category_no, UpdateActCategory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT act_no, category_no, order_no, subject, content, img_url FROM update_act_category WHERE act_no = %d AND category_no = %d",
        act_no, category_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(UpdateActCategory));
    record->act_no = row[0] ? atoi(row[0]) : 0;
    record->category_no = row[1] ? atoi(row[1]) : 0;
    record->order_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->subject, row[3], sizeof(record->subject) - 1);
    if (row[4]) strncpy(record->content, row[4], sizeof(record->content) - 1);
    if (row[5]) strncpy(record->img_url, row[5], sizeof(record->img_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int UpdateActCategory_GetAll(DBConnectionManager* manager, UpdateActCategory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT act_no, category_no, order_no, subject, content, img_url FROM update_act_category");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UpdateActCategory));
        records[count].act_no = row[0] ? atoi(row[0]) : 0;
        records[count].category_no = row[1] ? atoi(row[1]) : 0;
        records[count].order_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].subject, row[3], sizeof(records[count].subject) - 1);
        if (row[4]) strncpy(records[count].content, row[4], sizeof(records[count].content) - 1);
        if (row[5]) strncpy(records[count].img_url, row[5], sizeof(records[count].img_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
