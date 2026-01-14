#include "security_tip_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SecurityTip_Add(DBConnectionManager* manager, const SecurityTip* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO security_tip (degree, category, seq, title, reg_date, hists, m_id, user_id, mod_date, search_word, thumbnail_url, writer, summary, open_flag) "
        "VALUES (%d, %d, %d, '%s', %d, %d, %d, '%s', %d, '%s', '%s', '%s', '%s', %d)",
        record->degree, record->category, record->seq, record->title, record->reg_date, record->hists, record->m_id, record->user_id, record->mod_date, record->search_word, record->thumbnail_url, record->writer, record->summary, record->open_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SecurityTip_Get(DBConnectionManager* manager, int mz_id, SecurityTip* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT mz_id, degree, category, seq, title, reg_date, hists, m_id, user_id, mod_date, search_word, thumbnail_url, writer, summary, open_flag FROM security_tip WHERE mz_id = %d",
        mz_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SecurityTip));
    record->mz_id = row[0] ? atoi(row[0]) : 0;
    record->degree = row[1] ? atoi(row[1]) : 0;
    record->category = row[2] ? atoi(row[2]) : 0;
    record->seq = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->title, row[4], sizeof(record->title) - 1);
    record->reg_date = row[5] ? atoi(row[5]) : 0;
    record->hists = row[6] ? atoi(row[6]) : 0;
    record->m_id = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->user_id, row[8], sizeof(record->user_id) - 1);
    record->mod_date = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->search_word, row[10], sizeof(record->search_word) - 1);
    if (row[11]) strncpy(record->thumbnail_url, row[11], sizeof(record->thumbnail_url) - 1);
    if (row[12]) strncpy(record->writer, row[12], sizeof(record->writer) - 1);
    if (row[13]) strncpy(record->summary, row[13], sizeof(record->summary) - 1);
    record->open_flag = row[14] ? atoi(row[14]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SecurityTip_GetAll(DBConnectionManager* manager, SecurityTip* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT mz_id, degree, category, seq, title, reg_date, hists, m_id, user_id, mod_date, search_word, thumbnail_url, writer, summary, open_flag FROM security_tip");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SecurityTip));
        records[count].mz_id = row[0] ? atoi(row[0]) : 0;
        records[count].degree = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].seq = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].title, row[4], sizeof(records[count].title) - 1);
        records[count].reg_date = row[5] ? atoi(row[5]) : 0;
        records[count].hists = row[6] ? atoi(row[6]) : 0;
        records[count].m_id = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].user_id, row[8], sizeof(records[count].user_id) - 1);
        records[count].mod_date = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].search_word, row[10], sizeof(records[count].search_word) - 1);
        if (row[11]) strncpy(records[count].thumbnail_url, row[11], sizeof(records[count].thumbnail_url) - 1);
        if (row[12]) strncpy(records[count].writer, row[12], sizeof(records[count].writer) - 1);
        if (row[13]) strncpy(records[count].summary, row[13], sizeof(records[count].summary) - 1);
        records[count].open_flag = row[14] ? atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
