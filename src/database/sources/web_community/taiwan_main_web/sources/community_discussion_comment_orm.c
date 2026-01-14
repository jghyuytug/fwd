#include "community_discussion_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommunityDiscussionComment_Add(DBConnectionManager* manager, const CommunityDiscussionComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO community_discussion_comment (content_no, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, comment_text, ip, create_day) "
        "VALUES (%d, %d, '%s', '%s', %d, %d, '%s', %d, '%s', '%s', %d)",
        record->content_no, record->m_id, record->m_user_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->comment_text, record->ip, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionComment_Get(DBConnectionManager* manager, int no, CommunityDiscussionComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, comment_text, ip, create_day FROM community_discussion_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommunityDiscussionComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->m_user_id, row[3], sizeof(record->m_user_id) - 1);
    if (row[4]) strncpy(record->m_sex, row[4], sizeof(record->m_sex) - 1);
    record->server_id = row[5] ? atoi(row[5]) : 0;
    record->charac_no = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->charac_name, row[7], sizeof(record->charac_name) - 1);
    record->job = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->comment_text, row[9], sizeof(record->comment_text) - 1);
    if (row[10]) strncpy(record->ip, row[10], sizeof(record->ip) - 1);
    record->create_day = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionComment_GetAll(DBConnectionManager* manager, CommunityDiscussionComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, comment_text, ip, create_day FROM community_discussion_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommunityDiscussionComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].m_user_id, row[3], sizeof(records[count].m_user_id) - 1);
        if (row[4]) strncpy(records[count].m_sex, row[4], sizeof(records[count].m_sex) - 1);
        records[count].server_id = row[5] ? atoi(row[5]) : 0;
        records[count].charac_no = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].charac_name, row[7], sizeof(records[count].charac_name) - 1);
        records[count].job = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].comment_text, row[9], sizeof(records[count].comment_text) - 1);
        if (row[10]) strncpy(records[count].ip, row[10], sizeof(records[count].ip) - 1);
        records[count].create_day = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
