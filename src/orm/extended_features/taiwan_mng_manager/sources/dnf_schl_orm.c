#include "dnf_schl_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfSchl_Add(DBConnectionManager* manager, const DnfSchl* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_schl (server_group, end_date, st_date, schl_cd, period_month, period_week, period_day, period_hour, period_minute, type, wave_hour, wave_minute, level, stat, description, occ_date) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->server_group, record->end_date, record->st_date, record->schl_cd, record->period_month, record->period_week, record->period_day, record->period_hour, record->period_minute, record->type, record->wave_hour, record->wave_minute, record->level, record->stat, record->description, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchl_Get(DBConnectionManager* manager, signed char server_group, const char* end_date, const char* st_date, const char* schl_cd, DnfSchl* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, end_date, st_date, schl_cd, period_month, period_week, period_day, period_hour, period_minute, type, wave_hour, wave_minute, level, stat, description, occ_date FROM dnf_schl WHERE server_group = %d AND end_date = '%s' AND st_date = '%s' AND schl_cd = '%s'",
        server_group, end_date, st_date, schl_cd);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfSchl));
    record->server_group = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->end_date, row[1], sizeof(record->end_date) - 1);
    if (row[2]) strncpy(record->st_date, row[2], sizeof(record->st_date) - 1);
    if (row[3]) strncpy(record->schl_cd, row[3], sizeof(record->schl_cd) - 1);
    if (row[4]) strncpy(record->period_month, row[4], sizeof(record->period_month) - 1);
    if (row[5]) strncpy(record->period_week, row[5], sizeof(record->period_week) - 1);
    if (row[6]) strncpy(record->period_day, row[6], sizeof(record->period_day) - 1);
    if (row[7]) strncpy(record->period_hour, row[7], sizeof(record->period_hour) - 1);
    if (row[8]) strncpy(record->period_minute, row[8], sizeof(record->period_minute) - 1);
    if (row[9]) strncpy(record->type, row[9], sizeof(record->type) - 1);
    if (row[10]) strncpy(record->wave_hour, row[10], sizeof(record->wave_hour) - 1);
    if (row[11]) strncpy(record->wave_minute, row[11], sizeof(record->wave_minute) - 1);
    if (row[12]) strncpy(record->level, row[12], sizeof(record->level) - 1);
    if (row[13]) strncpy(record->stat, row[13], sizeof(record->stat) - 1);
    if (row[14]) strncpy(record->description, row[14], sizeof(record->description) - 1);
    if (row[15]) strncpy(record->occ_date, row[15], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchl_GetAll(DBConnectionManager* manager, DnfSchl* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, end_date, st_date, schl_cd, period_month, period_week, period_day, period_hour, period_minute, type, wave_hour, wave_minute, level, stat, description, occ_date FROM dnf_schl");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfSchl));
        records[count].server_group = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].end_date, row[1], sizeof(records[count].end_date) - 1);
        if (row[2]) strncpy(records[count].st_date, row[2], sizeof(records[count].st_date) - 1);
        if (row[3]) strncpy(records[count].schl_cd, row[3], sizeof(records[count].schl_cd) - 1);
        if (row[4]) strncpy(records[count].period_month, row[4], sizeof(records[count].period_month) - 1);
        if (row[5]) strncpy(records[count].period_week, row[5], sizeof(records[count].period_week) - 1);
        if (row[6]) strncpy(records[count].period_day, row[6], sizeof(records[count].period_day) - 1);
        if (row[7]) strncpy(records[count].period_hour, row[7], sizeof(records[count].period_hour) - 1);
        if (row[8]) strncpy(records[count].period_minute, row[8], sizeof(records[count].period_minute) - 1);
        if (row[9]) strncpy(records[count].type, row[9], sizeof(records[count].type) - 1);
        if (row[10]) strncpy(records[count].wave_hour, row[10], sizeof(records[count].wave_hour) - 1);
        if (row[11]) strncpy(records[count].wave_minute, row[11], sizeof(records[count].wave_minute) - 1);
        if (row[12]) strncpy(records[count].level, row[12], sizeof(records[count].level) - 1);
        if (row[13]) strncpy(records[count].stat, row[13], sizeof(records[count].stat) - 1);
        if (row[14]) strncpy(records[count].description, row[14], sizeof(records[count].description) - 1);
        if (row[15]) strncpy(records[count].occ_date, row[15], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
