#include "dnf_artifact_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfArtifactInfo_Add(DBConnectionManager* manager, const DnfArtifactInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_artifact_info (it_id, creature_min_level, physical_attack, magical_attack, skill_consume_mp_rate, skill_charge_time_rate, skill_overcharge_time_rate, experience_amount, physical_critical_hit, magical_critical_hit, stuck) "
        "VALUES (%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f)",
        record->it_id, record->creature_min_level, record->physical_attack, record->magical_attack, record->skill_consume_mp_rate, record->skill_charge_time_rate, record->skill_overcharge_time_rate, record->experience_amount, record->physical_critical_hit, record->magical_critical_hit, record->stuck);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfArtifactInfo_Get(DBConnectionManager* manager, int it_id, DnfArtifactInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, creature_min_level, physical_attack, magical_attack, skill_consume_mp_rate, skill_charge_time_rate, skill_overcharge_time_rate, experience_amount, physical_critical_hit, magical_critical_hit, stuck FROM dnf_artifact_info WHERE it_id = %d",
        it_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfArtifactInfo));
    record->it_id = row[0] ? atoi(row[0]) : 0;
    record->creature_min_level = row[1] ? atoi(row[1]) : 0;
    record->physical_attack = row[2] ? atoi(row[2]) : 0;
    record->magical_attack = row[3] ? atoi(row[3]) : 0;
    record->skill_consume_mp_rate = row[4] ? atof(row[4]) : 0;
    record->skill_charge_time_rate = row[5] ? atof(row[5]) : 0;
    record->skill_overcharge_time_rate = row[6] ? atof(row[6]) : 0;
    record->experience_amount = row[7] ? atof(row[7]) : 0;
    record->physical_critical_hit = row[8] ? atof(row[8]) : 0;
    record->magical_critical_hit = row[9] ? atof(row[9]) : 0;
    record->stuck = row[10] ? atof(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfArtifactInfo_GetAll(DBConnectionManager* manager, DnfArtifactInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, creature_min_level, physical_attack, magical_attack, skill_consume_mp_rate, skill_charge_time_rate, skill_overcharge_time_rate, experience_amount, physical_critical_hit, magical_critical_hit, stuck FROM dnf_artifact_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfArtifactInfo));
        records[count].it_id = row[0] ? atoi(row[0]) : 0;
        records[count].creature_min_level = row[1] ? atoi(row[1]) : 0;
        records[count].physical_attack = row[2] ? atoi(row[2]) : 0;
        records[count].magical_attack = row[3] ? atoi(row[3]) : 0;
        records[count].skill_consume_mp_rate = row[4] ? atof(row[4]) : 0;
        records[count].skill_charge_time_rate = row[5] ? atof(row[5]) : 0;
        records[count].skill_overcharge_time_rate = row[6] ? atof(row[6]) : 0;
        records[count].experience_amount = row[7] ? atof(row[7]) : 0;
        records[count].physical_critical_hit = row[8] ? atof(row[8]) : 0;
        records[count].magical_critical_hit = row[9] ? atof(row[9]) : 0;
        records[count].stuck = row[10] ? atof(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
