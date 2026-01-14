#include "powerclub_discussion_comment_recom_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PowerclubDiscussionCommentRecom_Add(DBConnectionManager* manager, const PowerclubDiscussionCommentRecom* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO powerclub_discussion_comment_recom (comment_no, m_id, content_no) "
        "VALUES (%d, %d, %d)",
        record->comment_no, record->m_id, record->content_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PowerclubDiscussionCommentRecom_Get(DBConnectionManager* manager, int no, PowerclubDiscussionCommentRecom* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, comment_no, m_id, content_no FROM powerclub_discussion_comment_recom WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PowerclubDiscussionCommentRecom));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->comment_no = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->content_no = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PowerclubDiscussionCommentRecom_GetAll(DBConnectionManager* manager, PowerclubDiscussionCommentRecom* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, comment_no, m_id, content_no FROM powerclub_discussion_comment_recom");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PowerclubDiscussionCommentRecom));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].comment_no = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].content_no = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
