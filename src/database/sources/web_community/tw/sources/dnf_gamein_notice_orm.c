#include "dnf_gamein_notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfGameinNotice_Add(DBConnectionManager* manager, const DnfGameinNotice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_gamein_notice (img_name, server_id, reg_time, open_flag) "
        "VALUES ('%s', %d, '%s', %d)",
        record->img_name, record->server_id, record->reg_time, record->open_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_Get(DBConnectionManager* manager, int no, DnfGameinNotice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, img_name, server_id, reg_time, open_flag FROM dnf_gamein_notice WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfGameinNotice));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->img_name, row[1], sizeof(record->img_name) - 1);
    record->server_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->reg_time, row[3], sizeof(record->reg_time) - 1);
    record->open_flag = row[4] ? row[4][0] : '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_GetAll(DBConnectionManager* manager, DnfGameinNotice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, img_name, server_id, reg_time, open_flag FROM dnf_gamein_notice");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfGameinNotice));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].img_name, row[1], sizeof(records[count].img_name) - 1);
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].reg_time, row[3], sizeof(records[count].reg_time) - 1);
        records[count].open_flag = row[4] ? row[4][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
