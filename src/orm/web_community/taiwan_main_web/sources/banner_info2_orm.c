#include "banner_info2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BannerInfo2_Add(DBConnectionManager* manager, const BannerInfo2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO banner_info2 (useyn, priority, img_url, target_url, reg_date, intro, no) "
        "VALUES (%d, %d, '%s', '%s', %d, '%s', %d)",
        record->useyn, record->priority, record->img_url, record->target_url, record->reg_date, record->intro, record->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BannerInfo2_Get(DBConnectionManager* manager, int id, BannerInfo2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, useyn, priority, img_url, target_url, reg_date, intro, no FROM banner_info2 WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(BannerInfo2));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->useyn = row[1] ? atoi(row[1]) : 0;
    record->priority = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->img_url, row[3], sizeof(record->img_url) - 1);
    if (row[4]) strncpy(record->target_url, row[4], sizeof(record->target_url) - 1);
    record->reg_date = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->intro, row[6], sizeof(record->intro) - 1);
    record->no = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int BannerInfo2_GetAll(DBConnectionManager* manager, BannerInfo2* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, useyn, priority, img_url, target_url, reg_date, intro, no FROM banner_info2");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BannerInfo2));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].useyn = row[1] ? atoi(row[1]) : 0;
        records[count].priority = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].img_url, row[3], sizeof(records[count].img_url) - 1);
        if (row[4]) strncpy(records[count].target_url, row[4], sizeof(records[count].target_url) - 1);
        records[count].reg_date = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].intro, row[6], sizeof(records[count].intro) - 1);
        records[count].no = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
