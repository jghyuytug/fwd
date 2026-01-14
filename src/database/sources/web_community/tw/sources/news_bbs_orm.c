#include "news_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int NewsBbs_Add(DBConnectionManager* manager, const NewsBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO news_bbs (bbs_code, emph_yn, user_id, reg_date, html_yn, subject, body, hits, prev_no, next_no, updt_date, use_yn, file_name) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', '%s', %d, %d, %d, %d, %d, '%s')",
        record->bbs_code, record->emph_yn, record->user_id, record->reg_date, record->html_yn, record->subject, record->body, record->hits, record->prev_no, record->next_no, record->updt_date, record->use_yn, record->file_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_Get(DBConnectionManager* manager, signed char bbs_code, signed char emph_yn, int no, NewsBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, emph_yn, no, user_id, reg_date, html_yn, subject, body, hits, prev_no, next_no, updt_date, use_yn, file_name FROM news_bbs WHERE bbs_code = %d AND emph_yn = %d AND no = %d",
        bbs_code, emph_yn, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(NewsBbs));
    record->bbs_code = row[0] ? atoi(row[0]) : 0;
    record->emph_yn = row[1] ? atoi(row[1]) : 0;
    record->no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->user_id, row[3], sizeof(record->user_id) - 1);
    record->reg_date = row[4] ? atoi(row[4]) : 0;
    record->html_yn = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->subject, row[6], sizeof(record->subject) - 1);
    if (row[7]) strncpy(record->body, row[7], sizeof(record->body) - 1);
    record->hits = row[8] ? atoi(row[8]) : 0;
    record->prev_no = row[9] ? atoi(row[9]) : 0;
    record->next_no = row[10] ? atoi(row[10]) : 0;
    record->updt_date = row[11] ? atoi(row[11]) : 0;
    record->use_yn = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->file_name, row[13], sizeof(record->file_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_GetAll(DBConnectionManager* manager, NewsBbs* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, emph_yn, no, user_id, reg_date, html_yn, subject, body, hits, prev_no, next_no, updt_date, use_yn, file_name FROM news_bbs");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(NewsBbs));
        records[count].bbs_code = row[0] ? atoi(row[0]) : 0;
        records[count].emph_yn = row[1] ? atoi(row[1]) : 0;
        records[count].no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].user_id, row[3], sizeof(records[count].user_id) - 1);
        records[count].reg_date = row[4] ? atoi(row[4]) : 0;
        records[count].html_yn = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].subject, row[6], sizeof(records[count].subject) - 1);
        if (row[7]) strncpy(records[count].body, row[7], sizeof(records[count].body) - 1);
        records[count].hits = row[8] ? atoi(row[8]) : 0;
        records[count].prev_no = row[9] ? atoi(row[9]) : 0;
        records[count].next_no = row[10] ? atoi(row[10]) : 0;
        records[count].updt_date = row[11] ? atoi(row[11]) : 0;
        records[count].use_yn = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].file_name, row[13], sizeof(records[count].file_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
