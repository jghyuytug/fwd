#include "dnf_mobile_board_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfMobileBoard_Add(DBConnectionManager* manager, const DnfMobileBoard* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_mobile_board (cate, open_flag, title, summary, thumbnail_img, view, comment, occ_time) "
        "VALUES (%d, %d, '%s', '%s', '%s', %d, %d, '%s')",
        record->cate, record->open_flag, record->title, record->summary, record->thumbnail_img, record->view, record->comment, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMobileBoard_Get(DBConnectionManager* manager, int dmb_idx, DnfMobileBoard* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT dmb_idx, cate, open_flag, title, summary, thumbnail_img, view, comment, occ_time FROM dnf_mobile_board WHERE dmb_idx = %d",
        dmb_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfMobileBoard));
    record->dmb_idx = row[0] ? atoi(row[0]) : 0;
    record->cate = row[1] ? atoi(row[1]) : 0;
    record->open_flag = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->title, row[3], sizeof(record->title) - 1);
    if (row[4]) strncpy(record->summary, row[4], sizeof(record->summary) - 1);
    if (row[5]) strncpy(record->thumbnail_img, row[5], sizeof(record->thumbnail_img) - 1);
    record->view = row[6] ? atoi(row[6]) : 0;
    record->comment = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->occ_time, row[8], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMobileBoard_GetAll(DBConnectionManager* manager, DnfMobileBoard* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT dmb_idx, cate, open_flag, title, summary, thumbnail_img, view, comment, occ_time FROM dnf_mobile_board");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfMobileBoard));
        records[count].dmb_idx = row[0] ? atoi(row[0]) : 0;
        records[count].cate = row[1] ? atoi(row[1]) : 0;
        records[count].open_flag = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].title, row[3], sizeof(records[count].title) - 1);
        if (row[4]) strncpy(records[count].summary, row[4], sizeof(records[count].summary) - 1);
        if (row[5]) strncpy(records[count].thumbnail_img, row[5], sizeof(records[count].thumbnail_img) - 1);
        records[count].view = row[6] ? atoi(row[6]) : 0;
        records[count].comment = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].occ_time, row[8], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
