#include "discussion_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DiscussionManager_Add(DBConnectionManager* manager, const DiscussionManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO discussion_manager (content_no, reg_date, content_name, content, positive_cmt, negative_cmt) "
        "VALUES (%d, %d, '%s', '%s', '%s', '%s')",
        record->content_no, record->reg_date, record->content_name, record->content, record->positive_cmt, record->negative_cmt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DiscussionManager_Get(DBConnectionManager* manager, int no, DiscussionManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, reg_date, content_name, content, positive_cmt, negative_cmt FROM discussion_manager WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DiscussionManager));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    record->reg_date = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->content_name, row[3], sizeof(record->content_name) - 1);
    if (row[4]) strncpy(record->content, row[4], sizeof(record->content) - 1);
    if (row[5]) strncpy(record->positive_cmt, row[5], sizeof(record->positive_cmt) - 1);
    if (row[6]) strncpy(record->negative_cmt, row[6], sizeof(record->negative_cmt) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DiscussionManager_GetAll(DBConnectionManager* manager, DiscussionManager* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, reg_date, content_name, content, positive_cmt, negative_cmt FROM discussion_manager");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DiscussionManager));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        records[count].reg_date = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].content_name, row[3], sizeof(records[count].content_name) - 1);
        if (row[4]) strncpy(records[count].content, row[4], sizeof(records[count].content) - 1);
        if (row[5]) strncpy(records[count].positive_cmt, row[5], sizeof(records[count].positive_cmt) - 1);
        if (row[6]) strncpy(records[count].negative_cmt, row[6], sizeof(records[count].negative_cmt) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
