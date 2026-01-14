#include "banner_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BannerInfo_Add(DBConnectionManager* manager, const BannerInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO banner_info (useyn, priority, img_url, target_url, title, reg_date, intro, no, banner_type, banner_type_sub, start_date, end_date) "
        "VALUES (%d, %d, '%s', '%s', '%s', %d, '%s', %d, %d, %d, %d, %d)",
        record->useyn, record->priority, record->img_url, record->target_url, record->title, record->reg_date, record->intro, record->no, record->banner_type, record->banner_type_sub, record->start_date, record->end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BannerInfo_Get(DBConnectionManager* manager, int id, BannerInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, useyn, priority, img_url, target_url, title, reg_date, intro, no, banner_type, banner_type_sub, start_date, end_date FROM banner_info WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(BannerInfo));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->useyn = row[1] ? atoi(row[1]) : 0;
    record->priority = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->img_url, row[3], sizeof(record->img_url) - 1);
    if (row[4]) strncpy(record->target_url, row[4], sizeof(record->target_url) - 1);
    if (row[5]) strncpy(record->title, row[5], sizeof(record->title) - 1);
    record->reg_date = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->intro, row[7], sizeof(record->intro) - 1);
    record->no = row[8] ? atoi(row[8]) : 0;
    record->banner_type = row[9] ? atoi(row[9]) : 0;
    record->banner_type_sub = row[10] ? atoi(row[10]) : 0;
    record->start_date = row[11] ? atoi(row[11]) : 0;
    record->end_date = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int BannerInfo_GetAll(DBConnectionManager* manager, BannerInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, useyn, priority, img_url, target_url, title, reg_date, intro, no, banner_type, banner_type_sub, start_date, end_date FROM banner_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BannerInfo));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].useyn = row[1] ? atoi(row[1]) : 0;
        records[count].priority = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].img_url, row[3], sizeof(records[count].img_url) - 1);
        if (row[4]) strncpy(records[count].target_url, row[4], sizeof(records[count].target_url) - 1);
        if (row[5]) strncpy(records[count].title, row[5], sizeof(records[count].title) - 1);
        records[count].reg_date = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].intro, row[7], sizeof(records[count].intro) - 1);
        records[count].no = row[8] ? atoi(row[8]) : 0;
        records[count].banner_type = row[9] ? atoi(row[9]) : 0;
        records[count].banner_type_sub = row[10] ? atoi(row[10]) : 0;
        records[count].start_date = row[11] ? atoi(row[11]) : 0;
        records[count].end_date = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
