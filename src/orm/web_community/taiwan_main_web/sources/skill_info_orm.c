#include "skill_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SkillInfo_Add(DBConnectionManager* manager, const SkillInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO skill_info (job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->job_index, record->skill_index, record->module_type, record->name, record->name2, record->basic_explain, record->skill_explain, record->purchase_cost, record->type, record->skill_class, record->growtype_maximum_level, record->second_growtype_maximum_level, record->skill_fitness_growtype, record->skill_fitness_second_growtype, record->consume_item, record->required_level, record->required_level_range, record->pre_required_skill, record->consume_mp, record->cool_time, record->casting_time, record->icon, record->command_key_explain, record->skill_command_advantage, record->static_data, record->level_info, record->start_cool_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SkillInfo_Get(DBConnectionManager* manager, int job_index, int skill_index, signed char module_type, SkillInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[27];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time FROM skill_info WHERE job_index = %d AND skill_index = %d AND module_type = %d",
        job_index, skill_index, module_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SkillInfo));
    record->job_index = row[0] ? atoi(row[0]) : 0;
    record->skill_index = row[1] ? atoi(row[1]) : 0;
    record->module_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->name, row[3], sizeof(record->name) - 1);
    if (row[4]) strncpy(record->name2, row[4], sizeof(record->name2) - 1);
    if (row[5]) strncpy(record->basic_explain, row[5], sizeof(record->basic_explain) - 1);
    if (row[6]) strncpy(record->skill_explain, row[6], sizeof(record->skill_explain) - 1);
    if (row[7]) strncpy(record->purchase_cost, row[7], sizeof(record->purchase_cost) - 1);
    record->type = row[8] ? atoi(row[8]) : 0;
    record->skill_class = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->growtype_maximum_level, row[10], sizeof(record->growtype_maximum_level) - 1);
    if (row[11]) strncpy(record->second_growtype_maximum_level, row[11], sizeof(record->second_growtype_maximum_level) - 1);
    if (row[12]) strncpy(record->skill_fitness_growtype, row[12], sizeof(record->skill_fitness_growtype) - 1);
    if (row[13]) strncpy(record->skill_fitness_second_growtype, row[13], sizeof(record->skill_fitness_second_growtype) - 1);
    if (row[14]) strncpy(record->consume_item, row[14], sizeof(record->consume_item) - 1);
    record->required_level = row[15] ? atoi(row[15]) : 0;
    record->required_level_range = row[16] ? atoi(row[16]) : 0;
    if (row[17]) strncpy(record->pre_required_skill, row[17], sizeof(record->pre_required_skill) - 1);
    if (row[18]) strncpy(record->consume_mp, row[18], sizeof(record->consume_mp) - 1);
    if (row[19]) strncpy(record->cool_time, row[19], sizeof(record->cool_time) - 1);
    if (row[20]) strncpy(record->casting_time, row[20], sizeof(record->casting_time) - 1);
    if (row[21]) strncpy(record->icon, row[21], sizeof(record->icon) - 1);
    if (row[22]) strncpy(record->command_key_explain, row[22], sizeof(record->command_key_explain) - 1);
    if (row[23]) strncpy(record->skill_command_advantage, row[23], sizeof(record->skill_command_advantage) - 1);
    if (row[24]) strncpy(record->static_data, row[24], sizeof(record->static_data) - 1);
    if (row[25]) strncpy(record->level_info, row[25], sizeof(record->level_info) - 1);
    if (row[26]) strncpy(record->start_cool_time, row[26], sizeof(record->start_cool_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SkillInfo_GetAll(DBConnectionManager* manager, SkillInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[27];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time FROM skill_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SkillInfo));
        records[count].job_index = row[0] ? atoi(row[0]) : 0;
        records[count].skill_index = row[1] ? atoi(row[1]) : 0;
        records[count].module_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].name, row[3], sizeof(records[count].name) - 1);
        if (row[4]) strncpy(records[count].name2, row[4], sizeof(records[count].name2) - 1);
        if (row[5]) strncpy(records[count].basic_explain, row[5], sizeof(records[count].basic_explain) - 1);
        if (row[6]) strncpy(records[count].skill_explain, row[6], sizeof(records[count].skill_explain) - 1);
        if (row[7]) strncpy(records[count].purchase_cost, row[7], sizeof(records[count].purchase_cost) - 1);
        records[count].type = row[8] ? atoi(row[8]) : 0;
        records[count].skill_class = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].growtype_maximum_level, row[10], sizeof(records[count].growtype_maximum_level) - 1);
        if (row[11]) strncpy(records[count].second_growtype_maximum_level, row[11], sizeof(records[count].second_growtype_maximum_level) - 1);
        if (row[12]) strncpy(records[count].skill_fitness_growtype, row[12], sizeof(records[count].skill_fitness_growtype) - 1);
        if (row[13]) strncpy(records[count].skill_fitness_second_growtype, row[13], sizeof(records[count].skill_fitness_second_growtype) - 1);
        if (row[14]) strncpy(records[count].consume_item, row[14], sizeof(records[count].consume_item) - 1);
        records[count].required_level = row[15] ? atoi(row[15]) : 0;
        records[count].required_level_range = row[16] ? atoi(row[16]) : 0;
        if (row[17]) strncpy(records[count].pre_required_skill, row[17], sizeof(records[count].pre_required_skill) - 1);
        if (row[18]) strncpy(records[count].consume_mp, row[18], sizeof(records[count].consume_mp) - 1);
        if (row[19]) strncpy(records[count].cool_time, row[19], sizeof(records[count].cool_time) - 1);
        if (row[20]) strncpy(records[count].casting_time, row[20], sizeof(records[count].casting_time) - 1);
        if (row[21]) strncpy(records[count].icon, row[21], sizeof(records[count].icon) - 1);
        if (row[22]) strncpy(records[count].command_key_explain, row[22], sizeof(records[count].command_key_explain) - 1);
        if (row[23]) strncpy(records[count].skill_command_advantage, row[23], sizeof(records[count].skill_command_advantage) - 1);
        if (row[24]) strncpy(records[count].static_data, row[24], sizeof(records[count].static_data) - 1);
        if (row[25]) strncpy(records[count].level_info, row[25], sizeof(records[count].level_info) - 1);
        if (row[26]) strncpy(records[count].start_cool_time, row[26], sizeof(records[count].start_cool_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
