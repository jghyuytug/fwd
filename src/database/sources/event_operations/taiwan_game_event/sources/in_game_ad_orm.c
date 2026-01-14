#include "in_game_ad_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int InGameAd_Add(DBConnectionManager* manager, const InGameAd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO in_game_ad (title, gender, age, job, level, banner_url, item_no, event_url, ctrl_banner_url, ctrl_event_url, ctrl_item_no, ctrl_description, min_sera, max_sera, type_code, visible, limit_m_id, view_code, image_url, description, log_flag, ex_property) "
        "VALUES ('%s', %d, %d, %d, '%s', '%s', %d, '%s', '%s', '%s', %d, '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', %d, '%s')",
        record->title, record->gender, record->age, record->job, record->level, record->banner_url, record->item_no, record->event_url, record->ctrl_banner_url, record->ctrl_event_url, record->ctrl_item_no, record->ctrl_description, record->min_sera, record->max_sera, record->type_code, record->visible, record->limit_m_id, record->view_code, record->image_url, record->description, record->log_flag, record->ex_property);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int InGameAd_Get(DBConnectionManager* manager, int banner_id, InGameAd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT banner_id, title, gender, age, job, level, banner_url, item_no, event_url, ctrl_banner_url, ctrl_event_url, ctrl_item_no, ctrl_description, min_sera, max_sera, type_code, visible, limit_m_id, view_code, image_url, description, log_flag, ex_property FROM in_game_ad WHERE banner_id = %d",
        banner_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(InGameAd));
    record->banner_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    record->gender = row[2] ? atoi(row[2]) : 0;
    record->age = row[3] ? atoi(row[3]) : 0;
    record->job = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->level, row[5], sizeof(record->level) - 1);
    if (row[6]) strncpy(record->banner_url, row[6], sizeof(record->banner_url) - 1);
    record->item_no = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->event_url, row[8], sizeof(record->event_url) - 1);
    if (row[9]) strncpy(record->ctrl_banner_url, row[9], sizeof(record->ctrl_banner_url) - 1);
    if (row[10]) strncpy(record->ctrl_event_url, row[10], sizeof(record->ctrl_event_url) - 1);
    record->ctrl_item_no = row[11] ? atoi(row[11]) : 0;
    if (row[12]) strncpy(record->ctrl_description, row[12], sizeof(record->ctrl_description) - 1);
    record->min_sera = row[13] ? atoi(row[13]) : 0;
    record->max_sera = row[14] ? atoi(row[14]) : 0;
    record->type_code = row[15] ? atoi(row[15]) : 0;
    record->visible = row[16] ? atoi(row[16]) : 0;
    record->limit_m_id = row[17] ? atoi(row[17]) : 0;
    record->view_code = row[18] ? atoi(row[18]) : 0;
    if (row[19]) strncpy(record->image_url, row[19], sizeof(record->image_url) - 1);
    if (row[20]) strncpy(record->description, row[20], sizeof(record->description) - 1);
    record->log_flag = row[21] ? atoi(row[21]) : 0;
    if (row[22]) strncpy(record->ex_property, row[22], sizeof(record->ex_property) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int InGameAd_GetAll(DBConnectionManager* manager, InGameAd* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT banner_id, title, gender, age, job, level, banner_url, item_no, event_url, ctrl_banner_url, ctrl_event_url, ctrl_item_no, ctrl_description, min_sera, max_sera, type_code, visible, limit_m_id, view_code, image_url, description, log_flag, ex_property FROM in_game_ad");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(InGameAd));
        records[count].banner_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        records[count].gender = row[2] ? atoi(row[2]) : 0;
        records[count].age = row[3] ? atoi(row[3]) : 0;
        records[count].job = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].level, row[5], sizeof(records[count].level) - 1);
        if (row[6]) strncpy(records[count].banner_url, row[6], sizeof(records[count].banner_url) - 1);
        records[count].item_no = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].event_url, row[8], sizeof(records[count].event_url) - 1);
        if (row[9]) strncpy(records[count].ctrl_banner_url, row[9], sizeof(records[count].ctrl_banner_url) - 1);
        if (row[10]) strncpy(records[count].ctrl_event_url, row[10], sizeof(records[count].ctrl_event_url) - 1);
        records[count].ctrl_item_no = row[11] ? atoi(row[11]) : 0;
        if (row[12]) strncpy(records[count].ctrl_description, row[12], sizeof(records[count].ctrl_description) - 1);
        records[count].min_sera = row[13] ? atoi(row[13]) : 0;
        records[count].max_sera = row[14] ? atoi(row[14]) : 0;
        records[count].type_code = row[15] ? atoi(row[15]) : 0;
        records[count].visible = row[16] ? atoi(row[16]) : 0;
        records[count].limit_m_id = row[17] ? atoi(row[17]) : 0;
        records[count].view_code = row[18] ? atoi(row[18]) : 0;
        if (row[19]) strncpy(records[count].image_url, row[19], sizeof(records[count].image_url) - 1);
        if (row[20]) strncpy(records[count].description, row[20], sizeof(records[count].description) - 1);
        records[count].log_flag = row[21] ? atoi(row[21]) : 0;
        if (row[22]) strncpy(records[count].ex_property, row[22], sizeof(records[count].ex_property) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
