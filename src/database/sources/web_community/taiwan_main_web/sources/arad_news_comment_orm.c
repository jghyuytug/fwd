#include "arad_news_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AradNewsComment_Add(DBConnectionManager* manager, const AradNewsComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO arad_news_comment (content_no, m_id, server_id, charac_no, charac_name, job, comment_text, create_day) "
        "VALUES (%d, %d, %d, %d, '%s', %d, '%s', %d)",
        record->content_no, record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->comment_text, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsComment_Get(DBConnectionManager* manager, int no, AradNewsComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, server_id, charac_no, charac_name, job, comment_text, create_day FROM arad_news_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AradNewsComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->server_id = row[3] ? atoi(row[3]) : 0;
    record->charac_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->charac_name, row[5], sizeof(record->charac_name) - 1);
    record->job = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->comment_text, row[7], sizeof(record->comment_text) - 1);
    record->create_day = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsComment_GetAll(DBConnectionManager* manager, AradNewsComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, server_id, charac_no, charac_name, job, comment_text, create_day FROM arad_news_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AradNewsComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].server_id = row[3] ? atoi(row[3]) : 0;
        records[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].charac_name, row[5], sizeof(records[count].charac_name) - 1);
        records[count].job = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].comment_text, row[7], sizeof(records[count].comment_text) - 1);
        records[count].create_day = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
