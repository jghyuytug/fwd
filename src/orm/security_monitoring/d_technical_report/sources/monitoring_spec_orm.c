#include "monitoring_spec_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MonitoringSpec_Add(DBConnectionManager* manager, const MonitoringSpec* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO monitoring_spec (modify_time, spec_id, cpu_vendor, cpu_processor_num, above_cpu_clock, below_cpu_clock, ram, videocard_vendor, videocard_device, videocard_texture_mem, os_version) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->modify_time, record->spec_id, record->cpu_vendor, record->cpu_processor_num, record->above_cpu_clock, record->below_cpu_clock, record->ram, record->videocard_vendor, record->videocard_device, record->videocard_texture_mem, record->os_version);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MonitoringSpec_Get(DBConnectionManager* manager, int unique_id, MonitoringSpec* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT unique_id, modify_time, spec_id, cpu_vendor, cpu_processor_num, above_cpu_clock, below_cpu_clock, ram, videocard_vendor, videocard_device, videocard_texture_mem, os_version FROM monitoring_spec WHERE unique_id = %d",
        unique_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MonitoringSpec));
    record->unique_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->modify_time, row[1], sizeof(record->modify_time) - 1);
    record->spec_id = row[2] ? atoi(row[2]) : 0;
    record->cpu_vendor = row[3] ? atoi(row[3]) : 0;
    record->cpu_processor_num = row[4] ? atoi(row[4]) : 0;
    record->above_cpu_clock = row[5] ? atoi(row[5]) : 0;
    record->below_cpu_clock = row[6] ? atoi(row[6]) : 0;
    record->ram = row[7] ? atoi(row[7]) : 0;
    record->videocard_vendor = row[8] ? atoi(row[8]) : 0;
    record->videocard_device = row[9] ? atoi(row[9]) : 0;
    record->videocard_texture_mem = row[10] ? atoi(row[10]) : 0;
    record->os_version = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MonitoringSpec_GetAll(DBConnectionManager* manager, MonitoringSpec* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT unique_id, modify_time, spec_id, cpu_vendor, cpu_processor_num, above_cpu_clock, below_cpu_clock, ram, videocard_vendor, videocard_device, videocard_texture_mem, os_version FROM monitoring_spec");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MonitoringSpec));
        records[count].unique_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].modify_time, row[1], sizeof(records[count].modify_time) - 1);
        records[count].spec_id = row[2] ? atoi(row[2]) : 0;
        records[count].cpu_vendor = row[3] ? atoi(row[3]) : 0;
        records[count].cpu_processor_num = row[4] ? atoi(row[4]) : 0;
        records[count].above_cpu_clock = row[5] ? atoi(row[5]) : 0;
        records[count].below_cpu_clock = row[6] ? atoi(row[6]) : 0;
        records[count].ram = row[7] ? atoi(row[7]) : 0;
        records[count].videocard_vendor = row[8] ? atoi(row[8]) : 0;
        records[count].videocard_device = row[9] ? atoi(row[9]) : 0;
        records[count].videocard_texture_mem = row[10] ? atoi(row[10]) : 0;
        records[count].os_version = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
