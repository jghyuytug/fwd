#include "month_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MonthItem_Add(DBConnectionManager* manager, const MonthItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO month_item (state, pre_img, now_img, next_img, admin_no, reg_date, pre_pop, now_pop, next_pop, pre_map, now_map, next_map) "
        "VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->state, record->pre_img, record->now_img, record->next_img, record->admin_no, record->reg_date, record->pre_pop, record->now_pop, record->next_pop, record->pre_map, record->now_map, record->next_map);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MonthItem_Get(DBConnectionManager* manager, int no, MonthItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, state, pre_img, now_img, next_img, admin_no, reg_date, pre_pop, now_pop, next_pop, pre_map, now_map, next_map FROM month_item WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MonthItem));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->state = row[1] ? row[1][0] : '\0';
    if (row[2]) strncpy(record->pre_img, row[2], sizeof(record->pre_img) - 1);
    if (row[3]) strncpy(record->now_img, row[3], sizeof(record->now_img) - 1);
    if (row[4]) strncpy(record->next_img, row[4], sizeof(record->next_img) - 1);
    record->admin_no = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->reg_date, row[6], sizeof(record->reg_date) - 1);
    if (row[7]) strncpy(record->pre_pop, row[7], sizeof(record->pre_pop) - 1);
    if (row[8]) strncpy(record->now_pop, row[8], sizeof(record->now_pop) - 1);
    if (row[9]) strncpy(record->next_pop, row[9], sizeof(record->next_pop) - 1);
    if (row[10]) strncpy(record->pre_map, row[10], sizeof(record->pre_map) - 1);
    if (row[11]) strncpy(record->now_map, row[11], sizeof(record->now_map) - 1);
    if (row[12]) strncpy(record->next_map, row[12], sizeof(record->next_map) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MonthItem_GetAll(DBConnectionManager* manager, MonthItem* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, state, pre_img, now_img, next_img, admin_no, reg_date, pre_pop, now_pop, next_pop, pre_map, now_map, next_map FROM month_item");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MonthItem));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].state = row[1] ? row[1][0] : '\0';
        if (row[2]) strncpy(records[count].pre_img, row[2], sizeof(records[count].pre_img) - 1);
        if (row[3]) strncpy(records[count].now_img, row[3], sizeof(records[count].now_img) - 1);
        if (row[4]) strncpy(records[count].next_img, row[4], sizeof(records[count].next_img) - 1);
        records[count].admin_no = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].reg_date, row[6], sizeof(records[count].reg_date) - 1);
        if (row[7]) strncpy(records[count].pre_pop, row[7], sizeof(records[count].pre_pop) - 1);
        if (row[8]) strncpy(records[count].now_pop, row[8], sizeof(records[count].now_pop) - 1);
        if (row[9]) strncpy(records[count].next_pop, row[9], sizeof(records[count].next_pop) - 1);
        if (row[10]) strncpy(records[count].pre_map, row[10], sizeof(records[count].pre_map) - 1);
        if (row[11]) strncpy(records[count].now_map, row[11], sizeof(records[count].now_map) - 1);
        if (row[12]) strncpy(records[count].next_map, row[12], sizeof(records[count].next_map) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
