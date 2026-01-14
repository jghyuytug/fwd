#include "todaydnf_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Todaydnf_Add(DBConnectionManager* manager, const Todaydnf* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO todaydnf (first_title, first_thumb, first_referer, first_url, first_hit, second_title, second_thumb, second_referer, second_url, second_hit, third_title, third_thumb, third_referer, third_url, third_hit, open_date, reg_date) "
        "VALUES ('%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', %d, '%s', '%s')",
        record->first_title, record->first_thumb, record->first_referer, record->first_url, record->first_hit, record->second_title, record->second_thumb, record->second_referer, record->second_url, record->second_hit, record->third_title, record->third_thumb, record->third_referer, record->third_url, record->third_hit, record->open_date, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Todaydnf_Get(DBConnectionManager* manager, int no, Todaydnf* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, first_title, first_thumb, first_referer, first_url, first_hit, second_title, second_thumb, second_referer, second_url, second_hit, third_title, third_thumb, third_referer, third_url, third_hit, open_date, reg_date FROM todaydnf WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Todaydnf));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->first_title, row[1], sizeof(record->first_title) - 1);
    if (row[2]) strncpy(record->first_thumb, row[2], sizeof(record->first_thumb) - 1);
    if (row[3]) strncpy(record->first_referer, row[3], sizeof(record->first_referer) - 1);
    if (row[4]) strncpy(record->first_url, row[4], sizeof(record->first_url) - 1);
    record->first_hit = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->second_title, row[6], sizeof(record->second_title) - 1);
    if (row[7]) strncpy(record->second_thumb, row[7], sizeof(record->second_thumb) - 1);
    if (row[8]) strncpy(record->second_referer, row[8], sizeof(record->second_referer) - 1);
    if (row[9]) strncpy(record->second_url, row[9], sizeof(record->second_url) - 1);
    record->second_hit = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->third_title, row[11], sizeof(record->third_title) - 1);
    if (row[12]) strncpy(record->third_thumb, row[12], sizeof(record->third_thumb) - 1);
    if (row[13]) strncpy(record->third_referer, row[13], sizeof(record->third_referer) - 1);
    if (row[14]) strncpy(record->third_url, row[14], sizeof(record->third_url) - 1);
    record->third_hit = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->open_date, row[16], sizeof(record->open_date) - 1);
    if (row[17]) strncpy(record->reg_date, row[17], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Todaydnf_GetAll(DBConnectionManager* manager, Todaydnf* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, first_title, first_thumb, first_referer, first_url, first_hit, second_title, second_thumb, second_referer, second_url, second_hit, third_title, third_thumb, third_referer, third_url, third_hit, open_date, reg_date FROM todaydnf");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Todaydnf));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].first_title, row[1], sizeof(records[count].first_title) - 1);
        if (row[2]) strncpy(records[count].first_thumb, row[2], sizeof(records[count].first_thumb) - 1);
        if (row[3]) strncpy(records[count].first_referer, row[3], sizeof(records[count].first_referer) - 1);
        if (row[4]) strncpy(records[count].first_url, row[4], sizeof(records[count].first_url) - 1);
        records[count].first_hit = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].second_title, row[6], sizeof(records[count].second_title) - 1);
        if (row[7]) strncpy(records[count].second_thumb, row[7], sizeof(records[count].second_thumb) - 1);
        if (row[8]) strncpy(records[count].second_referer, row[8], sizeof(records[count].second_referer) - 1);
        if (row[9]) strncpy(records[count].second_url, row[9], sizeof(records[count].second_url) - 1);
        records[count].second_hit = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].third_title, row[11], sizeof(records[count].third_title) - 1);
        if (row[12]) strncpy(records[count].third_thumb, row[12], sizeof(records[count].third_thumb) - 1);
        if (row[13]) strncpy(records[count].third_referer, row[13], sizeof(records[count].third_referer) - 1);
        if (row[14]) strncpy(records[count].third_url, row[14], sizeof(records[count].third_url) - 1);
        records[count].third_hit = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].open_date, row[16], sizeof(records[count].open_date) - 1);
        if (row[17]) strncpy(records[count].reg_date, row[17], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
