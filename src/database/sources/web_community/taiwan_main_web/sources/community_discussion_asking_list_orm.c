#include "community_discussion_asking_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommunityDiscussionAskingList_Add(DBConnectionManager* manager, const CommunityDiscussionAskingList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO community_discussion_asking_list (ask_no, recom, reg_date, content) "
        "VALUES (%d, %d, %d, '%s')",
        record->ask_no, record->recom, record->reg_date, record->content);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionAskingList_Get(DBConnectionManager* manager, int no, CommunityDiscussionAskingList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, ask_no, recom, reg_date, content FROM community_discussion_asking_list WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommunityDiscussionAskingList));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->ask_no = row[1] ? atoi(row[1]) : 0;
    record->recom = row[2] ? atoi(row[2]) : 0;
    record->reg_date = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->content, row[4], sizeof(record->content) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionAskingList_GetAll(DBConnectionManager* manager, CommunityDiscussionAskingList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, ask_no, recom, reg_date, content FROM community_discussion_asking_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommunityDiscussionAskingList));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].ask_no = row[1] ? atoi(row[1]) : 0;
        records[count].recom = row[2] ? atoi(row[2]) : 0;
        records[count].reg_date = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].content, row[4], sizeof(records[count].content) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
