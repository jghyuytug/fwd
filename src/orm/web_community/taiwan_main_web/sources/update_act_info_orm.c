#include "update_act_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UpdateActInfo_Add(DBConnectionManager* manager, const UpdateActInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO update_act_info (subject, MNG_user_id, reg_date, up_date, hits, open_flag, open_date, image_url) "
        "VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, '%s')",
        record->subject, record->MNG_user_id, record->reg_date, record->up_date, record->hits, record->open_flag, record->open_date, record->image_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UpdateActInfo_Get(DBConnectionManager* manager, int act_no, UpdateActInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT act_no, subject, MNG_user_id, reg_date, up_date, hits, open_flag, open_date, image_url FROM update_act_info WHERE act_no = %d",
        act_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(UpdateActInfo));
    record->act_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->subject, row[1], sizeof(record->subject) - 1);
    if (row[2]) strncpy(record->MNG_user_id, row[2], sizeof(record->MNG_user_id) - 1);
    if (row[3]) strncpy(record->reg_date, row[3], sizeof(record->reg_date) - 1);
    if (row[4]) strncpy(record->up_date, row[4], sizeof(record->up_date) - 1);
    record->hits = row[5] ? atoi(row[5]) : 0;
    record->open_flag = row[6] ? row[6][0] : '\0';
    record->open_date = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->image_url, row[8], sizeof(record->image_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int UpdateActInfo_GetAll(DBConnectionManager* manager, UpdateActInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT act_no, subject, MNG_user_id, reg_date, up_date, hits, open_flag, open_date, image_url FROM update_act_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UpdateActInfo));
        records[count].act_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].subject, row[1], sizeof(records[count].subject) - 1);
        if (row[2]) strncpy(records[count].MNG_user_id, row[2], sizeof(records[count].MNG_user_id) - 1);
        if (row[3]) strncpy(records[count].reg_date, row[3], sizeof(records[count].reg_date) - 1);
        if (row[4]) strncpy(records[count].up_date, row[4], sizeof(records[count].up_date) - 1);
        records[count].hits = row[5] ? atoi(row[5]) : 0;
        records[count].open_flag = row[6] ? row[6][0] : '\0';
        records[count].open_date = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].image_url, row[8], sizeof(records[count].image_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
