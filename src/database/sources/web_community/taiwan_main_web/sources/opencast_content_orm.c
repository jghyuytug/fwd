#include "opencast_content_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OpencastContent_Add(DBConnectionManager* manager, const OpencastContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO opencast_content (m_id, cate_no, cc_type, list_type, title, popular_word, link, view, img_url, img_view, occ_time, content_cnt, comment) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', '%s', %d, '%s', %d, '%s', %d, '%s')",
        record->m_id, record->cate_no, record->cc_type, record->list_type, record->title, record->popular_word, record->link, record->view, record->img_url, record->img_view, record->occ_time, record->content_cnt, record->comment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastContent_Get(DBConnectionManager* manager, int bbs_no, OpencastContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_no, m_id, cate_no, cc_type, list_type, title, popular_word, link, view, img_url, img_view, occ_time, content_cnt, comment FROM opencast_content WHERE bbs_no = %d",
        bbs_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OpencastContent));
    record->bbs_no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->cate_no = row[2] ? atoi(row[2]) : 0;
    record->cc_type = row[3] ? atoi(row[3]) : 0;
    record->list_type = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->title, row[5], sizeof(record->title) - 1);
    if (row[6]) strncpy(record->popular_word, row[6], sizeof(record->popular_word) - 1);
    if (row[7]) strncpy(record->link, row[7], sizeof(record->link) - 1);
    record->view = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->img_url, row[9], sizeof(record->img_url) - 1);
    record->img_view = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->occ_time, row[11], sizeof(record->occ_time) - 1);
    record->content_cnt = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->comment, row[13], sizeof(record->comment) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastContent_GetAll(DBConnectionManager* manager, OpencastContent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_no, m_id, cate_no, cc_type, list_type, title, popular_word, link, view, img_url, img_view, occ_time, content_cnt, comment FROM opencast_content");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OpencastContent));
        records[count].bbs_no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].cate_no = row[2] ? atoi(row[2]) : 0;
        records[count].cc_type = row[3] ? atoi(row[3]) : 0;
        records[count].list_type = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].title, row[5], sizeof(records[count].title) - 1);
        if (row[6]) strncpy(records[count].popular_word, row[6], sizeof(records[count].popular_word) - 1);
        if (row[7]) strncpy(records[count].link, row[7], sizeof(records[count].link) - 1);
        records[count].view = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].img_url, row[9], sizeof(records[count].img_url) - 1);
        records[count].img_view = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].occ_time, row[11], sizeof(records[count].occ_time) - 1);
        records[count].content_cnt = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].comment, row[13], sizeof(records[count].comment) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
