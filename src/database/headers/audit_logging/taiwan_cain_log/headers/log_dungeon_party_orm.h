#ifndef LOG_DUNGEON_PARTY_ORM_H
#define LOG_DUNGEON_PARTY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    int dungeon_index;
    int dungeon_diff;
    signed char dungeon_standard_level;
    signed char abuse_party;
    signed char balkun_party;
    int party_user_count;
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
    int fatigue_consume;
    int exp_add;
} LogDungeonParty;

/* CRUD Operations */
int LogDungeonParty_Add(DBConnectionManager* manager, const LogDungeonParty* record);
int LogDungeonParty_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int party_user_count, int success, LogDungeonParty* record);
int LogDungeonParty_Update(DBConnectionManager* manager, const LogDungeonParty* record);
int LogDungeonParty_Delete(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int party_user_count, int success);
int LogDungeonParty_Exists(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int party_user_count, int success);
int LogDungeonParty_GetAll(DBConnectionManager* manager, LogDungeonParty* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_PARTY_ORM_H */