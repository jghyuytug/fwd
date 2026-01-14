#include "used_memory_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UsedMemory_Add(DBConnectionManager* manager, const UsedMemory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO used_memory (occ_time, minute_type, module, memory) "
        "VALUES ('%s', '%s', '%s', %d)",
        record->occ_time, record->minute_type, record->module, record->memory);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UsedMemory_Get(DBConnectionManager* manager, const char* occ_time, const char* minute_type, const char* module, UsedMemory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, minute_type, module, memory FROM used_memory WHERE occ_time = '%s' AND minute_type = '%s' AND module = '%s'",
        occ_time, minute_type, module);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(UsedMemory));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    if (row[1]) strncpy(record->minute_type, row[1], sizeof(record->minute_type) - 1);
    if (row[2]) strncpy(record->module, row[2], sizeof(record->module) - 1);
    record->memory = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int UsedMemory_GetAll(DBConnectionManager* manager, UsedMemory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, minute_type, module, memory FROM used_memory");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UsedMemory));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        if (row[1]) strncpy(records[count].minute_type, row[1], sizeof(records[count].minute_type) - 1);
        if (row[2]) strncpy(records[count].module, row[2], sizeof(records[count].module) - 1);
        records[count].memory = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
