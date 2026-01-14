#include "knowcafe_plan_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafePlanComment_Add(DBConnectionManager* manager, const KnowcafePlanComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_plan_comment (content_no, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, comment_text, create_day) "
        "VALUES (%d, '%s', %d, '%s', %d, %d, %d, '%s', %d, '%s', %d)",
        record->content_no, record->m_nickname, record->m_id, record->m_user_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->comment_text, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafePlanComment_Get(DBConnectionManager* manager, int no, KnowcafePlanComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, comment_text, create_day FROM knowcafe_plan_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafePlanComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->m_nickname, row[2], sizeof(record->m_nickname) - 1);
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->m_user_id, row[4], sizeof(record->m_user_id) - 1);
    record->m_sex = row[5] ? row[5][0] : '\0';
    record->server_id = row[6] ? atoi(row[6]) : 0;
    record->charac_no = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->charac_name, row[8], sizeof(record->charac_name) - 1);
    record->job = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->comment_text, row[10], sizeof(record->comment_text) - 1);
    record->create_day = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafePlanComment_GetAll(DBConnectionManager* manager, KnowcafePlanComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, comment_text, create_day FROM knowcafe_plan_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafePlanComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].m_nickname, row[2], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].m_user_id, row[4], sizeof(records[count].m_user_id) - 1);
        records[count].m_sex = row[5] ? row[5][0] : '\0';
        records[count].server_id = row[6] ? atoi(row[6]) : 0;
        records[count].charac_no = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].charac_name, row[8], sizeof(records[count].charac_name) - 1);
        records[count].job = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].comment_text, row[10], sizeof(records[count].comment_text) - 1);
        records[count].create_day = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
