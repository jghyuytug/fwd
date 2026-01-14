#include "discussion_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DiscussionInfo_Add(DBConnectionManager* manager, const DiscussionInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO discussion_info (content_no, id, name, cnt) "
        "VALUES (%d, %d, '%s', %d)",
        record->content_no, record->id, record->name, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DiscussionInfo_Get(DBConnectionManager* manager, int content_no, short id, DiscussionInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT content_no, id, name, cnt FROM discussion_info WHERE content_no = %d AND id = %d",
        content_no, id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DiscussionInfo));
    record->content_no = row[0] ? atoi(row[0]) : 0;
    record->id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->name, row[2], sizeof(record->name) - 1);
    record->cnt = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DiscussionInfo_GetAll(DBConnectionManager* manager, DiscussionInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT content_no, id, name, cnt FROM discussion_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DiscussionInfo));
        records[count].content_no = row[0] ? atoi(row[0]) : 0;
        records[count].id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].name, row[2], sizeof(records[count].name) - 1);
        records[count].cnt = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
