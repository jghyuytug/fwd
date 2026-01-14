#include "intensive_discussion_comment_reply_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int IntensiveDiscussionCommentReply_Add(DBConnectionManager* manager, const IntensiveDiscussionCommentReply* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO intensive_discussion_comment_reply (no, m_nickname, m_id, m_user_id, server_id, charac_no, charac_name, job, comment_text, create_day, depth, sequence, del_flag, content_no) "
        "VALUES (%d, '%s', %d, '%s', %d, %d, '%s', %d, '%s', %d, %d, %d, %d, %d)",
        record->no, record->m_nickname, record->m_id, record->m_user_id, record->server_id, record->charac_no, record->charac_name, record->job, record->comment_text, record->create_day, record->depth, record->sequence, record->del_flag, record->content_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int IntensiveDiscussionCommentReply_Get(DBConnectionManager* manager, int rno, IntensiveDiscussionCommentReply* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, no, m_nickname, m_id, m_user_id, server_id, charac_no, charac_name, job, comment_text, create_day, depth, sequence, del_flag, content_no FROM intensive_discussion_comment_reply WHERE rno = %d",
        rno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(IntensiveDiscussionCommentReply));
    record->rno = row[0] ? atoi(row[0]) : 0;
    record->no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->m_nickname, row[2], sizeof(record->m_nickname) - 1);
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->m_user_id, row[4], sizeof(record->m_user_id) - 1);
    record->server_id = row[5] ? atoi(row[5]) : 0;
    record->charac_no = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->charac_name, row[7], sizeof(record->charac_name) - 1);
    record->job = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->comment_text, row[9], sizeof(record->comment_text) - 1);
    record->create_day = row[10] ? atoi(row[10]) : 0;
    record->depth = row[11] ? atoi(row[11]) : 0;
    record->sequence = row[12] ? atoi(row[12]) : 0;
    record->del_flag = row[13] ? atoi(row[13]) : 0;
    record->content_no = row[14] ? atoi(row[14]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int IntensiveDiscussionCommentReply_GetAll(DBConnectionManager* manager, IntensiveDiscussionCommentReply* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, no, m_nickname, m_id, m_user_id, server_id, charac_no, charac_name, job, comment_text, create_day, depth, sequence, del_flag, content_no FROM intensive_discussion_comment_reply");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(IntensiveDiscussionCommentReply));
        records[count].rno = row[0] ? atoi(row[0]) : 0;
        records[count].no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].m_nickname, row[2], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].m_user_id, row[4], sizeof(records[count].m_user_id) - 1);
        records[count].server_id = row[5] ? atoi(row[5]) : 0;
        records[count].charac_no = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].charac_name, row[7], sizeof(records[count].charac_name) - 1);
        records[count].job = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].comment_text, row[9], sizeof(records[count].comment_text) - 1);
        records[count].create_day = row[10] ? atoi(row[10]) : 0;
        records[count].depth = row[11] ? atoi(row[11]) : 0;
        records[count].sequence = row[12] ? atoi(row[12]) : 0;
        records[count].del_flag = row[13] ? atoi(row[13]) : 0;
        records[count].content_no = row[14] ? atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
