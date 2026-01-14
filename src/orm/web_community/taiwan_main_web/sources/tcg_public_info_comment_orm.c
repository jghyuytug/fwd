#include "tcg_public_info_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TcgPublicInfoComment_Add(DBConnectionManager* manager, const TcgPublicInfoComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tcg_public_info_comment (content_no, m_id, charac_no, charac_name, comment_text, ip, create_day) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', %d)",
        record->content_no, record->m_id, record->charac_no, record->charac_name, record->comment_text, record->ip, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TcgPublicInfoComment_Get(DBConnectionManager* manager, int no, TcgPublicInfoComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, charac_no, charac_name, comment_text, ip, create_day FROM tcg_public_info_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TcgPublicInfoComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->comment_text, row[5], sizeof(record->comment_text) - 1);
    if (row[6]) strncpy(record->ip, row[6], sizeof(record->ip) - 1);
    record->create_day = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int TcgPublicInfoComment_GetAll(DBConnectionManager* manager, TcgPublicInfoComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, m_id, charac_no, charac_name, comment_text, ip, create_day FROM tcg_public_info_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TcgPublicInfoComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].comment_text, row[5], sizeof(records[count].comment_text) - 1);
        if (row[6]) strncpy(records[count].ip, row[6], sizeof(records[count].ip) - 1);
        records[count].create_day = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
