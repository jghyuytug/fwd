#include "ting_user_spec_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TingUserSpec_Add(DBConnectionManager* manager, const TingUserSpec* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ting_user_spec (m_id, reg_datetime, cpu_vendor, cpu_num, cpu_clock, ram, video_vendor, video_device, video_ram, os, os_bit) "
        "VALUES (%d, '%s', '%s', '%s', %d, %d, %d, %d, %d, '%s', '%s')",
        record->m_id, record->reg_datetime, record->cpu_vendor, record->cpu_num, record->cpu_clock, record->ram, record->video_vendor, record->video_device, record->video_ram, record->os, record->os_bit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TingUserSpec_Get(DBConnectionManager* manager, int m_id, TingUserSpec* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_datetime, cpu_vendor, cpu_num, cpu_clock, ram, video_vendor, video_device, video_ram, os, os_bit FROM ting_user_spec WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TingUserSpec));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->reg_datetime, row[1], sizeof(record->reg_datetime) - 1);
    if (row[2]) strncpy(record->cpu_vendor, row[2], sizeof(record->cpu_vendor) - 1);
    if (row[3]) strncpy(record->cpu_num, row[3], sizeof(record->cpu_num) - 1);
    record->cpu_clock = row[4] ? atoi(row[4]) : 0;
    record->ram = row[5] ? atoi(row[5]) : 0;
    record->video_vendor = row[6] ? atoi(row[6]) : 0;
    record->video_device = row[7] ? atoi(row[7]) : 0;
    record->video_ram = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->os, row[9], sizeof(record->os) - 1);
    if (row[10]) strncpy(record->os_bit, row[10], sizeof(record->os_bit) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int TingUserSpec_GetAll(DBConnectionManager* manager, TingUserSpec* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_datetime, cpu_vendor, cpu_num, cpu_clock, ram, video_vendor, video_device, video_ram, os, os_bit FROM ting_user_spec");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TingUserSpec));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].reg_datetime, row[1], sizeof(records[count].reg_datetime) - 1);
        if (row[2]) strncpy(records[count].cpu_vendor, row[2], sizeof(records[count].cpu_vendor) - 1);
        if (row[3]) strncpy(records[count].cpu_num, row[3], sizeof(records[count].cpu_num) - 1);
        records[count].cpu_clock = row[4] ? atoi(row[4]) : 0;
        records[count].ram = row[5] ? atoi(row[5]) : 0;
        records[count].video_vendor = row[6] ? atoi(row[6]) : 0;
        records[count].video_device = row[7] ? atoi(row[7]) : 0;
        records[count].video_ram = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].os, row[9], sizeof(records[count].os) - 1);
        if (row[10]) strncpy(records[count].os_bit, row[10], sizeof(records[count].os_bit) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
