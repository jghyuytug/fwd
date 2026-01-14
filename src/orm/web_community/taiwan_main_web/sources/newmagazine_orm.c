#include "newmagazine_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Newmagazine_Add(DBConnectionManager* manager, const Newmagazine* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO newmagazine (degree, category, seq, title, reg_date, hists, m_id, user_id, server_id, charac_no, charac_name, mod_date, point, point_cnt, search_word, thumbnail_url, writer, summary, open_flag, open_date, comments) "
        "VALUES (%d, %d, %d, '%s', %d, %d, %d, '%s', %d, %d, '%s', %d, %d, %d, '%s', '%s', '%s', '%s', %d, %d, %d)",
        record->degree, record->category, record->seq, record->title, record->reg_date, record->hists, record->m_id, record->user_id, record->server_id, record->charac_no, record->charac_name, record->mod_date, record->point, record->point_cnt, record->search_word, record->thumbnail_url, record->writer, record->summary, record->open_flag, record->open_date, record->comments);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Newmagazine_Get(DBConnectionManager* manager, int mz_id, Newmagazine* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[22];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT mz_id, degree, category, seq, title, reg_date, hists, m_id, user_id, server_id, charac_no, charac_name, mod_date, point, point_cnt, search_word, thumbnail_url, writer, summary, open_flag, open_date, comments FROM newmagazine WHERE mz_id = %d",
        mz_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Newmagazine));
    record->mz_id = row[0] ? atoi(row[0]) : 0;
    record->degree = row[1] ? atoi(row[1]) : 0;
    record->category = row[2] ? atoi(row[2]) : 0;
    record->seq = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->title, row[4], sizeof(record->title) - 1);
    record->reg_date = row[5] ? atoi(row[5]) : 0;
    record->hists = row[6] ? atoi(row[6]) : 0;
    record->m_id = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->user_id, row[8], sizeof(record->user_id) - 1);
    record->server_id = row[9] ? atoi(row[9]) : 0;
    record->charac_no = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->charac_name, row[11], sizeof(record->charac_name) - 1);
    record->mod_date = row[12] ? atoi(row[12]) : 0;
    record->point = row[13] ? atoi(row[13]) : 0;
    record->point_cnt = row[14] ? atoi(row[14]) : 0;
    if (row[15]) strncpy(record->search_word, row[15], sizeof(record->search_word) - 1);
    if (row[16]) strncpy(record->thumbnail_url, row[16], sizeof(record->thumbnail_url) - 1);
    if (row[17]) strncpy(record->writer, row[17], sizeof(record->writer) - 1);
    if (row[18]) strncpy(record->summary, row[18], sizeof(record->summary) - 1);
    record->open_flag = row[19] ? atoi(row[19]) : 0;
    record->open_date = row[20] ? atoi(row[20]) : 0;
    record->comments = row[21] ? atoi(row[21]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Newmagazine_GetAll(DBConnectionManager* manager, Newmagazine* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[22];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT mz_id, degree, category, seq, title, reg_date, hists, m_id, user_id, server_id, charac_no, charac_name, mod_date, point, point_cnt, search_word, thumbnail_url, writer, summary, open_flag, open_date, comments FROM newmagazine");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Newmagazine));
        records[count].mz_id = row[0] ? atoi(row[0]) : 0;
        records[count].degree = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].seq = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].title, row[4], sizeof(records[count].title) - 1);
        records[count].reg_date = row[5] ? atoi(row[5]) : 0;
        records[count].hists = row[6] ? atoi(row[6]) : 0;
        records[count].m_id = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].user_id, row[8], sizeof(records[count].user_id) - 1);
        records[count].server_id = row[9] ? atoi(row[9]) : 0;
        records[count].charac_no = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].charac_name, row[11], sizeof(records[count].charac_name) - 1);
        records[count].mod_date = row[12] ? atoi(row[12]) : 0;
        records[count].point = row[13] ? atoi(row[13]) : 0;
        records[count].point_cnt = row[14] ? atoi(row[14]) : 0;
        if (row[15]) strncpy(records[count].search_word, row[15], sizeof(records[count].search_word) - 1);
        if (row[16]) strncpy(records[count].thumbnail_url, row[16], sizeof(records[count].thumbnail_url) - 1);
        if (row[17]) strncpy(records[count].writer, row[17], sizeof(records[count].writer) - 1);
        if (row[18]) strncpy(records[count].summary, row[18], sizeof(records[count].summary) - 1);
        records[count].open_flag = row[19] ? atoi(row[19]) : 0;
        records[count].open_date = row[20] ? atoi(row[20]) : 0;
        records[count].comments = row[21] ? atoi(row[21]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
