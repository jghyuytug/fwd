#ifndef LOG_DUNGEON_SKILL_ORM_H
#define LOG_DUNGEON_SKILL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    int dungeon_index;
    int dungeon_diff;
    int charac_job;
    int charac_grow;
    int skill_index;
    int skill_use;
    int skill_count;
    int skill_cool_time;
    char last_time[20];
} LogDungeonSkill;

/* CRUD Operations */
int LogDungeonSkill_Add(DBConnectionManager* manager, const LogDungeonSkill* record);
int LogDungeonSkill_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int charac_job, int charac_grow, int skill_index, LogDungeonSkill* record);
int LogDungeonSkill_Update(DBConnectionManager* manager, const LogDungeonSkill* record);
int LogDungeonSkill_Delete(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int charac_job, int charac_grow, int skill_index);
int LogDungeonSkill_Exists(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int charac_job, int charac_grow, int skill_index);
int LogDungeonSkill_GetAll(DBConnectionManager* manager, LogDungeonSkill* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_SKILL_ORM_H */