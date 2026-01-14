#include "community_discussion_asking_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommunityDiscussionAsking_Add(DBConnectionManager* manager, const CommunityDiscussionAsking* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO community_discussion_asking (title, main_title, content, content_type, admin_no, start_date, end_date, reg_date, modi_date, open, main_open, view, comment) "
        "VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->title, record->main_title, record->content, record->content_type, record->admin_no, record->start_date, record->end_date, record->reg_date, record->modi_date, record->open, record->main_open, record->view, record->comment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionAsking_Get(DBConnectionManager* manager, int no, CommunityDiscussionAsking* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, main_title, content, content_type, admin_no, start_date, end_date, reg_date, modi_date, open, main_open, view, comment FROM community_discussion_asking WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommunityDiscussionAsking));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    if (row[2]) strncpy(record->main_title, row[2], sizeof(record->main_title) - 1);
    if (row[3]) strncpy(record->content, row[3], sizeof(record->content) - 1);
    if (row[4]) strncpy(record->content_type, row[4], sizeof(record->content_type) - 1);
    record->admin_no = row[5] ? atoi(row[5]) : 0;
    record->start_date = row[6] ? atoi(row[6]) : 0;
    record->end_date = row[7] ? atoi(row[7]) : 0;
    record->reg_date = row[8] ? atoi(row[8]) : 0;
    record->modi_date = row[9] ? atoi(row[9]) : 0;
    record->open = row[10] ? atoi(row[10]) : 0;
    record->main_open = row[11] ? atoi(row[11]) : 0;
    record->view = row[12] ? atoi(row[12]) : 0;
    record->comment = row[13] ? atoi(row[13]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionAsking_GetAll(DBConnectionManager* manager, CommunityDiscussionAsking* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, main_title, content, content_type, admin_no, start_date, end_date, reg_date, modi_date, open, main_open, view, comment FROM community_discussion_asking");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommunityDiscussionAsking));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        if (row[2]) strncpy(records[count].main_title, row[2], sizeof(records[count].main_title) - 1);
        if (row[3]) strncpy(records[count].content, row[3], sizeof(records[count].content) - 1);
        if (row[4]) strncpy(records[count].content_type, row[4], sizeof(records[count].content_type) - 1);
        records[count].admin_no = row[5] ? atoi(row[5]) : 0;
        records[count].start_date = row[6] ? atoi(row[6]) : 0;
        records[count].end_date = row[7] ? atoi(row[7]) : 0;
        records[count].reg_date = row[8] ? atoi(row[8]) : 0;
        records[count].modi_date = row[9] ? atoi(row[9]) : 0;
        records[count].open = row[10] ? atoi(row[10]) : 0;
        records[count].main_open = row[11] ? atoi(row[11]) : 0;
        records[count].view = row[12] ? atoi(row[12]) : 0;
        records[count].comment = row[13] ? atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
