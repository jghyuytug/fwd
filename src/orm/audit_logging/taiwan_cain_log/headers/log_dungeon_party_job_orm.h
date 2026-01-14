#ifndef LOG_DUNGEON_PARTY_JOB_ORM_H
#define LOG_DUNGEON_PARTY_JOB_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    int dungeon_index;
    int dungeon_diff;
    signed char dungeon_standard_level;
    signed char abuse_party;
    signed char balkun_party;
    int charac_job;
    int charac_grow;
    int party_user_count;
    int success;
    char last_time[20];
    int job_count;
    int rank;
} LogDungeonPartyJob;

/* CRUD Operations */
int LogDungeonPartyJob_Add(DBConnectionManager* manager, const LogDungeonPartyJob* record);
int LogDungeonPartyJob_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int charac_job, int charac_grow, int party_user_count, int success, LogDungeonPartyJob* record);
int LogDungeonPartyJob_Update(DBConnectionManager* manager, const LogDungeonPartyJob* record);
int LogDungeonPartyJob_Delete(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int charac_job, int charac_grow, int party_user_count, int success);
int LogDungeonPartyJob_Exists(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int charac_job, int charac_grow, int party_user_count, int success);
int LogDungeonPartyJob_GetAll(DBConnectionManager* manager, LogDungeonPartyJob* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_PARTY_JOB_ORM_H */