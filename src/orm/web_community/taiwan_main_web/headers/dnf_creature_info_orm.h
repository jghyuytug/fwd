#ifndef DNF_CREATURE_INFO_ORM_H
#define DNF_CREATURE_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_id;
    short creature_id;
    char creature_name[65];
    int skill_recovery_time;
    int overskill_recovery_time;
    char artifact_slot[4];
    short learn_overskill_level;
    char skill_info[33];
    char overskill_info[33];
    char piercing[33];
    char skill_name[33];
    char skill_desc[256];
    char overskill_name[33];
    char overskill_desc[256];
    char skill_level_values[49];
    char overskill_level_values[49];
    short evolution_creature_id;
    short evolution_level;
} DnfCreatureInfo;

/* CRUD Operations */
int DnfCreatureInfo_Add(DBConnectionManager* manager, const DnfCreatureInfo* record);
int DnfCreatureInfo_Get(DBConnectionManager* manager, int it_id, DnfCreatureInfo* record);
int DnfCreatureInfo_Update(DBConnectionManager* manager, const DnfCreatureInfo* record);
int DnfCreatureInfo_Delete(DBConnectionManager* manager, int it_id);
int DnfCreatureInfo_Exists(DBConnectionManager* manager, int it_id);
int DnfCreatureInfo_GetAll(DBConnectionManager* manager, DnfCreatureInfo* records, int max_count, int* actual_count);

#endif /* DNF_CREATURE_INFO_ORM_H */