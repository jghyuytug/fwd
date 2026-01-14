#include "guide_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideBbs_Add(DBConnectionManager* manager, const GuideBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_bbs (title, bbs_cd, occ_time, img_src, url, view_cnt, click_cnt, open_flg) "
        "VALUES ('%s', %d, %d, '%s', '%s', %d, %d, %d)",
        record->title, record->bbs_cd, record->occ_time, record->img_src, record->url, record->view_cnt, record->click_cnt, record->open_flg);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbs_Get(DBConnectionManager* manager, int no, GuideBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, bbs_cd, occ_time, img_src, url, view_cnt, click_cnt, open_flg FROM guide_bbs WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideBbs));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    record->bbs_cd = row[2] ? atoi(row[2]) : 0;
    record->occ_time = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->img_src, row[4], sizeof(record->img_src) - 1);
    if (row[5]) strncpy(record->url, row[5], sizeof(record->url) - 1);
    record->view_cnt = row[6] ? atoi(row[6]) : 0;
    record->click_cnt = row[7] ? atoi(row[7]) : 0;
    record->open_flg = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbs_GetAll(DBConnectionManager* manager, GuideBbs* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, title, bbs_cd, occ_time, img_src, url, view_cnt, click_cnt, open_flg FROM guide_bbs");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideBbs));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        records[count].bbs_cd = row[2] ? atoi(row[2]) : 0;
        records[count].occ_time = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].img_src, row[4], sizeof(records[count].img_src) - 1);
        if (row[5]) strncpy(records[count].url, row[5], sizeof(records[count].url) - 1);
        records[count].view_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].click_cnt = row[7] ? atoi(row[7]) : 0;
        records[count].open_flg = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
