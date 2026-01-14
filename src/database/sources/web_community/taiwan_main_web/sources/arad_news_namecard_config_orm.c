#include "arad_news_namecard_config_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AradNewsNamecardConfig_Add(DBConnectionManager* manager, const AradNewsNamecardConfig* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO arad_news_namecard_config (m_id, server_id, charac_no, charac_name, job, lev, referer_url, blog_bgcolor) "
        "VALUES (%d, %d, %d, '%s', %d, %d, '%s', %d)",
        record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->lev, record->referer_url, record->blog_bgcolor);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsNamecardConfig_Get(DBConnectionManager* manager, int m_id, AradNewsNamecardConfig* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, charac_name, job, lev, referer_url, blog_bgcolor FROM arad_news_namecard_config WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AradNewsNamecardConfig));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->charac_name, row[3], sizeof(record->charac_name) - 1);
    record->job = row[4] ? atoi(row[4]) : 0;
    record->lev = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->referer_url, row[6], sizeof(record->referer_url) - 1);
    record->blog_bgcolor = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsNamecardConfig_GetAll(DBConnectionManager* manager, AradNewsNamecardConfig* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, charac_name, job, lev, referer_url, blog_bgcolor FROM arad_news_namecard_config");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AradNewsNamecardConfig));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
        records[count].job = row[4] ? atoi(row[4]) : 0;
        records[count].lev = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].referer_url, row[6], sizeof(records[count].referer_url) - 1);
        records[count].blog_bgcolor = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
