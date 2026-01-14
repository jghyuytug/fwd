#include "arad_news_tail_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AradNewsTailComment_Add(DBConnectionManager* manager, const AradNewsTailComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO arad_news_tail_comment (m_id, server_id, charac_no, charac_name, job, lev, comment, guild_id, guild_name, school_id, school_name, userimg_path, blog_url) "
        "VALUES (%d, %d, %d, '%s', %d, %d, '%s', %d, '%s', %d, '%s', '%s', '%s')",
        record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->lev, record->comment, record->guild_id, record->guild_name, record->school_id, record->school_name, record->userimg_path, record->blog_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsTailComment_Get(DBConnectionManager* manager, int m_id, AradNewsTailComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, charac_name, job, lev, comment, guild_id, guild_name, school_id, school_name, userimg_path, blog_url FROM arad_news_tail_comment WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AradNewsTailComment));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->charac_name, row[3], sizeof(record->charac_name) - 1);
    record->job = row[4] ? atoi(row[4]) : 0;
    record->lev = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->comment, row[6], sizeof(record->comment) - 1);
    record->guild_id = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->guild_name, row[8], sizeof(record->guild_name) - 1);
    record->school_id = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->school_name, row[10], sizeof(record->school_name) - 1);
    if (row[11]) strncpy(record->userimg_path, row[11], sizeof(record->userimg_path) - 1);
    if (row[12]) strncpy(record->blog_url, row[12], sizeof(record->blog_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsTailComment_GetAll(DBConnectionManager* manager, AradNewsTailComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, charac_name, job, lev, comment, guild_id, guild_name, school_id, school_name, userimg_path, blog_url FROM arad_news_tail_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AradNewsTailComment));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
        records[count].job = row[4] ? atoi(row[4]) : 0;
        records[count].lev = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].comment, row[6], sizeof(records[count].comment) - 1);
        records[count].guild_id = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].guild_name, row[8], sizeof(records[count].guild_name) - 1);
        records[count].school_id = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].school_name, row[10], sizeof(records[count].school_name) - 1);
        if (row[11]) strncpy(records[count].userimg_path, row[11], sizeof(records[count].userimg_path) - 1);
        if (row[12]) strncpy(records[count].blog_url, row[12], sizeof(records[count].blog_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
