#include "knowledge_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowledgeComment_Add(DBConnectionManager* manager, const KnowledgeComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowledge_comment (content_no, m_id, server_id, charac_no, charac_name, job, comment_text, ip, create_day, value) "
        "VALUES (%d, %d, %d, %d, '%s', %d, '%s', '%s', %d, %d)",
        record->content_no, record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->comment_text, record->ip, record->create_day, record->value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowledgeComment_Get(DBConnectionManager* manager, int no, KnowledgeComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, server_id, charac_no, charac_name, job, comment_text, ip, create_day, value FROM knowledge_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowledgeComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->server_id = row[3] ? atoi(row[3]) : 0;
    record->charac_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->charac_name, row[5], sizeof(record->charac_name) - 1);
    record->job = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->comment_text, row[7], sizeof(record->comment_text) - 1);
    if (row[8]) strncpy(record->ip, row[8], sizeof(record->ip) - 1);
    record->create_day = row[9] ? atoi(row[9]) : 0;
    record->value = row[10] ? atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowledgeComment_GetAll(DBConnectionManager* manager, KnowledgeComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, server_id, charac_no, charac_name, job, comment_text, ip, create_day, value FROM knowledge_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowledgeComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].server_id = row[3] ? atoi(row[3]) : 0;
        records[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].charac_name, row[5], sizeof(records[count].charac_name) - 1);
        records[count].job = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].comment_text, row[7], sizeof(records[count].comment_text) - 1);
        if (row[8]) strncpy(records[count].ip, row[8], sizeof(records[count].ip) - 1);
        records[count].create_day = row[9] ? atoi(row[9]) : 0;
        records[count].value = row[10] ? atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
