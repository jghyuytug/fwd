#ifndef SKILL_INFO_ORM_H
#define SKILL_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int job_index;
    int skill_index;
    signed char module_type;
    char name[256];
    char name2[256];
    char basic_explain[256];
    char skill_explain[256];
    char purchase_cost[256];
    signed char type;
    int skill_class;
    char growtype_maximum_level[256];
    char second_growtype_maximum_level[256];
    char skill_fitness_growtype[256];
    char skill_fitness_second_growtype[256];
    char consume_item[256];
    signed char required_level;
    signed char required_level_range;
    char pre_required_skill[256];
    char consume_mp[256];
    char cool_time[256];
    char casting_time[256];
    char icon[256];
    char command_key_explain[256];
    char skill_command_advantage[256];
    char static_data[256];
    char level_info[1024];
    char start_cool_time[256];
} SkillInfo;

/* CRUD Operations */
int SkillInfo_Add(DBConnectionManager* manager, const SkillInfo* record);
int SkillInfo_Get(DBConnectionManager* manager, int job_index, int skill_index, signed char module_type, SkillInfo* record);
int SkillInfo_Update(DBConnectionManager* manager, const SkillInfo* record);
int SkillInfo_Delete(DBConnectionManager* manager, int job_index, int skill_index, signed char module_type);
int SkillInfo_Exists(DBConnectionManager* manager, int job_index, int skill_index, signed char module_type);
int SkillInfo_GetAll(DBConnectionManager* manager, SkillInfo* records, int max_count, int* actual_count);

#endif /* SKILL_INFO_ORM_H */