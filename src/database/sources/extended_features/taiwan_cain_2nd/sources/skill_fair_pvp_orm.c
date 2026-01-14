#include "skill_fair_pvp_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SkillFairPvp_Add(DBConnectionManager* manager, const SkillFairPvp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO skill_fair_pvp (charac_no, remain_sp, sp_garbage, used_sp, lethe_flag, remain_sp_2nd, lethe_flag_2nd, remain_sfp_1st, remain_sfp_2nd, script_version) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->charac_no, record->remain_sp, record->sp_garbage, record->used_sp, record->lethe_flag, record->remain_sp_2nd, record->lethe_flag_2nd, record->remain_sfp_1st, record->remain_sfp_2nd, record->script_version);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SkillFairPvp_Get(DBConnectionManager* manager, int charac_no, SkillFairPvp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, remain_sp, sp_garbage, used_sp, lethe_flag, remain_sp_2nd, lethe_flag_2nd, remain_sfp_1st, remain_sfp_2nd, script_version FROM skill_fair_pvp WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SkillFairPvp));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->remain_sp = row[1] ? atoi(row[1]) : 0;
    record->sp_garbage = row[2] ? atoi(row[2]) : 0;
    record->used_sp = row[3] ? atoi(row[3]) : 0;
    record->lethe_flag = row[4] ? atoi(row[4]) : 0;
    record->remain_sp_2nd = row[5] ? atoi(row[5]) : 0;
    record->lethe_flag_2nd = row[6] ? atoi(row[6]) : 0;
    record->remain_sfp_1st = row[7] ? atoi(row[7]) : 0;
    record->remain_sfp_2nd = row[8] ? atoi(row[8]) : 0;
    record->script_version = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SkillFairPvp_GetAll(DBConnectionManager* manager, SkillFairPvp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, remain_sp, sp_garbage, used_sp, lethe_flag, remain_sp_2nd, lethe_flag_2nd, remain_sfp_1st, remain_sfp_2nd, script_version FROM skill_fair_pvp");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SkillFairPvp));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].remain_sp = row[1] ? atoi(row[1]) : 0;
        records[count].sp_garbage = row[2] ? atoi(row[2]) : 0;
        records[count].used_sp = row[3] ? atoi(row[3]) : 0;
        records[count].lethe_flag = row[4] ? atoi(row[4]) : 0;
        records[count].remain_sp_2nd = row[5] ? atoi(row[5]) : 0;
        records[count].lethe_flag_2nd = row[6] ? atoi(row[6]) : 0;
        records[count].remain_sfp_1st = row[7] ? atoi(row[7]) : 0;
        records[count].remain_sfp_2nd = row[8] ? atoi(row[8]) : 0;
        records[count].script_version = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
