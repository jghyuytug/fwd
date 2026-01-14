#include "arad_news_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AradNews_Add(DBConnectionManager* manager, const AradNews* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO arad_news (notice, category, m_id, server_id, charac_no, charac_name, job, lev, title, create_day, comment, view, recom, source_url, copyright, tag, no_recom, is_main) "
        "VALUES (%d, %d, %d, %d, %d, '%s', %d, %d, '%s', %d, %d, %d, %d, '%s', %d, '%s', %d, %d)",
        record->notice, record->category, record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->lev, record->title, record->create_day, record->comment, record->view, record->recom, record->source_url, record->copyright, record->tag, record->no_recom, record->is_main);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AradNews_Get(DBConnectionManager* manager, int no, AradNews* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, m_id, server_id, charac_no, charac_name, job, lev, title, create_day, comment, view, recom, source_url, copyright, tag, no_recom, is_main FROM arad_news WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AradNews));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->notice = row[1] ? row[1][0] : '\0';
    record->category = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    record->server_id = row[4] ? atoi(row[4]) : 0;
    record->charac_no = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->charac_name, row[6], sizeof(record->charac_name) - 1);
    record->job = row[7] ? atoi(row[7]) : 0;
    record->lev = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->title, row[9], sizeof(record->title) - 1);
    record->create_day = row[10] ? atoi(row[10]) : 0;
    record->comment = row[11] ? atoi(row[11]) : 0;
    record->view = row[12] ? atoi(row[12]) : 0;
    record->recom = row[13] ? atoi(row[13]) : 0;
    if (row[14]) strncpy(record->source_url, row[14], sizeof(record->source_url) - 1);
    record->copyright = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->tag, row[16], sizeof(record->tag) - 1);
    record->no_recom = row[17] ? atoi(row[17]) : 0;
    record->is_main = row[18] ? atoi(row[18]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AradNews_GetAll(DBConnectionManager* manager, AradNews* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, m_id, server_id, charac_no, charac_name, job, lev, title, create_day, comment, view, recom, source_url, copyright, tag, no_recom, is_main FROM arad_news");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AradNews));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].notice = row[1] ? row[1][0] : '\0';
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        records[count].server_id = row[4] ? atoi(row[4]) : 0;
        records[count].charac_no = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].charac_name, row[6], sizeof(records[count].charac_name) - 1);
        records[count].job = row[7] ? atoi(row[7]) : 0;
        records[count].lev = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].title, row[9], sizeof(records[count].title) - 1);
        records[count].create_day = row[10] ? atoi(row[10]) : 0;
        records[count].comment = row[11] ? atoi(row[11]) : 0;
        records[count].view = row[12] ? atoi(row[12]) : 0;
        records[count].recom = row[13] ? atoi(row[13]) : 0;
        if (row[14]) strncpy(records[count].source_url, row[14], sizeof(records[count].source_url) - 1);
        records[count].copyright = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].tag, row[16], sizeof(records[count].tag) - 1);
        records[count].no_recom = row[17] ? atoi(row[17]) : 0;
        records[count].is_main = row[18] ? atoi(row[18]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
