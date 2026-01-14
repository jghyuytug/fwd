#include "dnf_creature_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfCreatureInfo_Add(DBConnectionManager* manager, const DnfCreatureInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_creature_info (it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d)",
        record->it_id, record->creature_id, record->creature_name, record->skill_recovery_time, record->overskill_recovery_time, record->artifact_slot, record->learn_overskill_level, record->skill_info, record->overskill_info, record->piercing, record->skill_name, record->skill_desc, record->overskill_name, record->overskill_desc, record->skill_level_values, record->overskill_level_values, record->evolution_creature_id, record->evolution_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCreatureInfo_Get(DBConnectionManager* manager, int it_id, DnfCreatureInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level FROM dnf_creature_info WHERE it_id = %d",
        it_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfCreatureInfo));
    record->it_id = row[0] ? atoi(row[0]) : 0;
    record->creature_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->creature_name, row[2], sizeof(record->creature_name) - 1);
    record->skill_recovery_time = row[3] ? atoi(row[3]) : 0;
    record->overskill_recovery_time = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->artifact_slot, row[5], sizeof(record->artifact_slot) - 1);
    record->learn_overskill_level = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->skill_info, row[7], sizeof(record->skill_info) - 1);
    if (row[8]) strncpy(record->overskill_info, row[8], sizeof(record->overskill_info) - 1);
    if (row[9]) strncpy(record->piercing, row[9], sizeof(record->piercing) - 1);
    if (row[10]) strncpy(record->skill_name, row[10], sizeof(record->skill_name) - 1);
    if (row[11]) strncpy(record->skill_desc, row[11], sizeof(record->skill_desc) - 1);
    if (row[12]) strncpy(record->overskill_name, row[12], sizeof(record->overskill_name) - 1);
    if (row[13]) strncpy(record->overskill_desc, row[13], sizeof(record->overskill_desc) - 1);
    if (row[14]) strncpy(record->skill_level_values, row[14], sizeof(record->skill_level_values) - 1);
    if (row[15]) strncpy(record->overskill_level_values, row[15], sizeof(record->overskill_level_values) - 1);
    record->evolution_creature_id = row[16] ? atoi(row[16]) : 0;
    record->evolution_level = row[17] ? atoi(row[17]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCreatureInfo_GetAll(DBConnectionManager* manager, DnfCreatureInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level FROM dnf_creature_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfCreatureInfo));
        records[count].it_id = row[0] ? atoi(row[0]) : 0;
        records[count].creature_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].creature_name, row[2], sizeof(records[count].creature_name) - 1);
        records[count].skill_recovery_time = row[3] ? atoi(row[3]) : 0;
        records[count].overskill_recovery_time = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].artifact_slot, row[5], sizeof(records[count].artifact_slot) - 1);
        records[count].learn_overskill_level = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].skill_info, row[7], sizeof(records[count].skill_info) - 1);
        if (row[8]) strncpy(records[count].overskill_info, row[8], sizeof(records[count].overskill_info) - 1);
        if (row[9]) strncpy(records[count].piercing, row[9], sizeof(records[count].piercing) - 1);
        if (row[10]) strncpy(records[count].skill_name, row[10], sizeof(records[count].skill_name) - 1);
        if (row[11]) strncpy(records[count].skill_desc, row[11], sizeof(records[count].skill_desc) - 1);
        if (row[12]) strncpy(records[count].overskill_name, row[12], sizeof(records[count].overskill_name) - 1);
        if (row[13]) strncpy(records[count].overskill_desc, row[13], sizeof(records[count].overskill_desc) - 1);
        if (row[14]) strncpy(records[count].skill_level_values, row[14], sizeof(records[count].skill_level_values) - 1);
        if (row[15]) strncpy(records[count].overskill_level_values, row[15], sizeof(records[count].overskill_level_values) - 1);
        records[count].evolution_creature_id = row[16] ? atoi(row[16]) : 0;
        records[count].evolution_level = row[17] ? atoi(row[17]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
