#include "dnf_tune_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfTune_Add(DBConnectionManager* manager, const DnfTune* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_tune (link_tag, img_name, create_time, apply_type) "
        "VALUES ('%s', '%s', %d, '%s')",
        record->link_tag, record->img_name, record->create_time, record->apply_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTune_Get(DBConnectionManager* manager, int no, DnfTune* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, link_tag, img_name, create_time, apply_type FROM dnf_tune WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfTune));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->link_tag, row[1], sizeof(record->link_tag) - 1);
    if (row[2]) strncpy(record->img_name, row[2], sizeof(record->img_name) - 1);
    record->create_time = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->apply_type, row[4], sizeof(record->apply_type) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTune_GetAll(DBConnectionManager* manager, DnfTune* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, link_tag, img_name, create_time, apply_type FROM dnf_tune");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfTune));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].link_tag, row[1], sizeof(records[count].link_tag) - 1);
        if (row[2]) strncpy(records[count].img_name, row[2], sizeof(records[count].img_name) - 1);
        records[count].create_time = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].apply_type, row[4], sizeof(records[count].apply_type) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
