#include "dnf_schl_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfSchlLog_Add(DBConnectionManager* manager, const DnfSchlLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_schl_log (occ_time, server_group, schl_cd, end_date, st_date, stat, description) "
        "VALUES ('%s', %d, '%s', '%s', '%s', '%s', '%s')",
        record->occ_time, record->server_group, record->schl_cd, record->end_date, record->st_date, record->stat, record->description);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchlLog_Get(DBConnectionManager* manager, int schl_log_id, DnfSchlLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT schl_log_id, occ_time, server_group, schl_cd, end_date, st_date, stat, description FROM dnf_schl_log WHERE schl_log_id = %d",
        schl_log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfSchlLog));
    record->schl_log_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->server_group = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->schl_cd, row[3], sizeof(record->schl_cd) - 1);
    if (row[4]) strncpy(record->end_date, row[4], sizeof(record->end_date) - 1);
    if (row[5]) strncpy(record->st_date, row[5], sizeof(record->st_date) - 1);
    if (row[6]) strncpy(record->stat, row[6], sizeof(record->stat) - 1);
    if (row[7]) strncpy(record->description, row[7], sizeof(record->description) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchlLog_GetAll(DBConnectionManager* manager, DnfSchlLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT schl_log_id, occ_time, server_group, schl_cd, end_date, st_date, stat, description FROM dnf_schl_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfSchlLog));
        records[count].schl_log_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].server_group = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].schl_cd, row[3], sizeof(records[count].schl_cd) - 1);
        if (row[4]) strncpy(records[count].end_date, row[4], sizeof(records[count].end_date) - 1);
        if (row[5]) strncpy(records[count].st_date, row[5], sizeof(records[count].st_date) - 1);
        if (row[6]) strncpy(records[count].stat, row[6], sizeof(records[count].stat) - 1);
        if (row[7]) strncpy(records[count].description, row[7], sizeof(records[count].description) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
