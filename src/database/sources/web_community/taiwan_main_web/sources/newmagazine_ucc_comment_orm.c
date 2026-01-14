#include "newmagazine_ucc_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int NewmagazineUccComment_Add(DBConnectionManager* manager, const NewmagazineUccComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO newmagazine_ucc_comment (mz_id, m_id, m_sex, server_id, charac_no, charac_name, job, comment_text, create_day) "
        "VALUES (%d, %d, %d, %d, %d, '%s', %d, '%s', %d)",
        record->mz_id, record->m_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->comment_text, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineUccComment_Get(DBConnectionManager* manager, int no, NewmagazineUccComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, mz_id, m_id, m_sex, server_id, charac_no, charac_name, job, comment_text, create_day FROM newmagazine_ucc_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(NewmagazineUccComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->mz_id = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->m_sex = row[3] ? row[3][0] : '\0';
    record->server_id = row[4] ? atoi(row[4]) : 0;
    record->charac_no = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->charac_name, row[6], sizeof(record->charac_name) - 1);
    record->job = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->comment_text, row[8], sizeof(record->comment_text) - 1);
    record->create_day = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineUccComment_GetAll(DBConnectionManager* manager, NewmagazineUccComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, mz_id, m_id, m_sex, server_id, charac_no, charac_name, job, comment_text, create_day FROM newmagazine_ucc_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(NewmagazineUccComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].mz_id = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].m_sex = row[3] ? row[3][0] : '\0';
        records[count].server_id = row[4] ? atoi(row[4]) : 0;
        records[count].charac_no = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].charac_name, row[6], sizeof(records[count].charac_name) - 1);
        records[count].job = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].comment_text, row[8], sizeof(records[count].comment_text) - 1);
        records[count].create_day = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
