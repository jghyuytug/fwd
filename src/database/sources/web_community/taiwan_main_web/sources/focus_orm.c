#include "focus_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Focus_Add(DBConnectionManager* manager, const Focus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO focus (reg_date, open_date, img_url, img_link, today_word, hot_title, hot_link, hot_content, hot_word, img_view, hot_view, comment) "
        "VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d)",
        record->reg_date, record->open_date, record->img_url, record->img_link, record->today_word, record->hot_title, record->hot_link, record->hot_content, record->hot_word, record->img_view, record->hot_view, record->comment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Focus_Get(DBConnectionManager* manager, int no, Focus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, reg_date, open_date, img_url, img_link, today_word, hot_title, hot_link, hot_content, hot_word, img_view, hot_view, comment FROM focus WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Focus));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->reg_date = row[1] ? atoi(row[1]) : 0;
    record->open_date = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->img_url, row[3], sizeof(record->img_url) - 1);
    if (row[4]) strncpy(record->img_link, row[4], sizeof(record->img_link) - 1);
    if (row[5]) strncpy(record->today_word, row[5], sizeof(record->today_word) - 1);
    if (row[6]) strncpy(record->hot_title, row[6], sizeof(record->hot_title) - 1);
    if (row[7]) strncpy(record->hot_link, row[7], sizeof(record->hot_link) - 1);
    if (row[8]) strncpy(record->hot_content, row[8], sizeof(record->hot_content) - 1);
    if (row[9]) strncpy(record->hot_word, row[9], sizeof(record->hot_word) - 1);
    record->img_view = row[10] ? atoi(row[10]) : 0;
    record->hot_view = row[11] ? atoi(row[11]) : 0;
    record->comment = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Focus_GetAll(DBConnectionManager* manager, Focus* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, reg_date, open_date, img_url, img_link, today_word, hot_title, hot_link, hot_content, hot_word, img_view, hot_view, comment FROM focus");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Focus));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].reg_date = row[1] ? atoi(row[1]) : 0;
        records[count].open_date = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].img_url, row[3], sizeof(records[count].img_url) - 1);
        if (row[4]) strncpy(records[count].img_link, row[4], sizeof(records[count].img_link) - 1);
        if (row[5]) strncpy(records[count].today_word, row[5], sizeof(records[count].today_word) - 1);
        if (row[6]) strncpy(records[count].hot_title, row[6], sizeof(records[count].hot_title) - 1);
        if (row[7]) strncpy(records[count].hot_link, row[7], sizeof(records[count].hot_link) - 1);
        if (row[8]) strncpy(records[count].hot_content, row[8], sizeof(records[count].hot_content) - 1);
        if (row[9]) strncpy(records[count].hot_word, row[9], sizeof(records[count].hot_word) - 1);
        records[count].img_view = row[10] ? atoi(row[10]) : 0;
        records[count].hot_view = row[11] ? atoi(row[11]) : 0;
        records[count].comment = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
