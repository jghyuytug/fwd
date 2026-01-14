#ifndef LOG_DUNGEON_CHARAC_ORM_H
#define LOG_DUNGEON_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    int dungeon_index;
    int dungeon_diff;
    int charac_job;
    int charac_grow;
    int clear_time;
    int die_count;
    int hp_consume;
    int mp_consume;
    int hit_count;
    int hit_per_avg_damage;
    int hp_recovery;
    int mp_recovery;
    int success;
    char last_time[20];
    int update_count;
    int level;
    int exp_avg;
    int dungeon_standard_level;
    int fatigue_consume;
    int exp_add;
    signed char party_user_count;
    int rank;
} LogDungeonCharac;

/* CRUD Operations */
int LogDungeonCharac_Add(DBConnectionManager* manager, const LogDungeonCharac* record);
int LogDungeonCharac_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int dungeon_standard_level, int charac_job, int charac_grow, int success, signed char party_user_count, LogDungeonCharac* record);
int LogDungeonCharac_Update(DBConnectionManager* manager, const LogDungeonCharac* record);
int LogDungeonCharac_Delete(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int dungeon_standard_level, int charac_job, int charac_grow, int success, signed char party_user_count);
int LogDungeonCharac_Exists(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int dungeon_standard_level, int charac_job, int charac_grow, int success, signed char party_user_count);
int LogDungeonCharac_GetAll(DBConnectionManager* manager, LogDungeonCharac* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_CHARAC_ORM_H */