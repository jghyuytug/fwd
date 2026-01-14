#include "opencast_content_sub_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OpencastContentSub_Add(DBConnectionManager* manager, const OpencastContentSub* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO opencast_content_sub (m_id, bbs_no, title, popular_word, link, view, img_url, img_view) "
        "VALUES (%d, %d, '%s', '%s', '%s', %d, '%s', %d)",
        record->m_id, record->bbs_no, record->title, record->popular_word, record->link, record->view, record->img_url, record->img_view);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastContentSub_Get(DBConnectionManager* manager, int no, OpencastContentSub* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bbs_no, title, popular_word, link, view, img_url, img_view FROM opencast_content_sub WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OpencastContentSub));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->bbs_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->title, row[3], sizeof(record->title) - 1);
    if (row[4]) strncpy(record->popular_word, row[4], sizeof(record->popular_word) - 1);
    if (row[5]) strncpy(record->link, row[5], sizeof(record->link) - 1);
    record->view = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->img_url, row[7], sizeof(record->img_url) - 1);
    record->img_view = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastContentSub_GetAll(DBConnectionManager* manager, OpencastContentSub* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bbs_no, title, popular_word, link, view, img_url, img_view FROM opencast_content_sub");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OpencastContentSub));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].bbs_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].title, row[3], sizeof(records[count].title) - 1);
        if (row[4]) strncpy(records[count].popular_word, row[4], sizeof(records[count].popular_word) - 1);
        if (row[5]) strncpy(records[count].link, row[5], sizeof(records[count].link) - 1);
        records[count].view = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].img_url, row[7], sizeof(records[count].img_url) - 1);
        records[count].img_view = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
