#include "guide_menu_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideMenu_Add(DBConnectionManager* manager, const GuideMenu* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_menu (bbs_no, url, title, sort_no, view_type, depth_1, depth_2, depth_3, depth_4) "
        "VALUES (%d, '%s', '%s', %d, %d, %d, %d, %d, %d)",
        record->bbs_no, record->url, record->title, record->sort_no, record->view_type, record->depth_1, record->depth_2, record->depth_3, record->depth_4);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideMenu_Get(DBConnectionManager* manager, int no, GuideMenu* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, url, title, sort_no, view_type, depth_1, depth_2, depth_3, depth_4 FROM guide_menu WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideMenu));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->bbs_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->url, row[2], sizeof(record->url) - 1);
    if (row[3]) strncpy(record->title, row[3], sizeof(record->title) - 1);
    record->sort_no = row[4] ? atoi(row[4]) : 0;
    record->view_type = row[5] ? atoi(row[5]) : 0;
    record->depth_1 = row[6] ? atoi(row[6]) : 0;
    record->depth_2 = row[7] ? atoi(row[7]) : 0;
    record->depth_3 = row[8] ? atoi(row[8]) : 0;
    record->depth_4 = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuideMenu_GetAll(DBConnectionManager* manager, GuideMenu* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, url, title, sort_no, view_type, depth_1, depth_2, depth_3, depth_4 FROM guide_menu");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideMenu));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].url, row[2], sizeof(records[count].url) - 1);
        if (row[3]) strncpy(records[count].title, row[3], sizeof(records[count].title) - 1);
        records[count].sort_no = row[4] ? atoi(row[4]) : 0;
        records[count].view_type = row[5] ? atoi(row[5]) : 0;
        records[count].depth_1 = row[6] ? atoi(row[6]) : 0;
        records[count].depth_2 = row[7] ? atoi(row[7]) : 0;
        records[count].depth_3 = row[8] ? atoi(row[8]) : 0;
        records[count].depth_4 = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
