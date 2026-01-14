#include "member_environment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberEnvironment_Add(DBConnectionManager* manager, const MemberEnvironment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_environment (os_type, hard_wholesize, hard_remainsize, memory_wholesize, memory_remainsize, virtualmem_wholesize, virtualmem_remainsize, cpu_speed, cpu_count, cpu_vendor, graphic_name, graphic_memory, average_fps, reg_date) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', %d, %d, %d)",
        record->os_type, record->hard_wholesize, record->hard_remainsize, record->memory_wholesize, record->memory_remainsize, record->virtualmem_wholesize, record->virtualmem_remainsize, record->cpu_speed, record->cpu_count, record->cpu_vendor, record->graphic_name, record->graphic_memory, record->average_fps, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberEnvironment_GetAll(DBConnectionManager* manager, MemberEnvironment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT os_type, hard_wholesize, hard_remainsize, memory_wholesize, memory_remainsize, virtualmem_wholesize, virtualmem_remainsize, cpu_speed, cpu_count, cpu_vendor, graphic_name, graphic_memory, average_fps, reg_date FROM member_environment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberEnvironment));
        records[count].os_type = row[0] ? atoi(row[0]) : 0;
        records[count].hard_wholesize = row[1] ? atoi(row[1]) : 0;
        records[count].hard_remainsize = row[2] ? atoi(row[2]) : 0;
        records[count].memory_wholesize = row[3] ? atoi(row[3]) : 0;
        records[count].memory_remainsize = row[4] ? atoi(row[4]) : 0;
        records[count].virtualmem_wholesize = row[5] ? atoi(row[5]) : 0;
        records[count].virtualmem_remainsize = row[6] ? atoi(row[6]) : 0;
        records[count].cpu_speed = row[7] ? atoi(row[7]) : 0;
        records[count].cpu_count = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].cpu_vendor, row[9], sizeof(records[count].cpu_vendor) - 1);
        if (row[10]) strncpy(records[count].graphic_name, row[10], sizeof(records[count].graphic_name) - 1);
        records[count].graphic_memory = row[11] ? atoi(row[11]) : 0;
        records[count].average_fps = row[12] ? atoi(row[12]) : 0;
        records[count].reg_date = row[13] ? atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
