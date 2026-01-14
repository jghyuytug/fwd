#ifndef DNF_ARTIFACT_INFO_ORM_H
#define DNF_ARTIFACT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_id;
    int creature_min_level;
    int physical_attack;
    int magical_attack;
    float skill_consume_mp_rate;
    float skill_charge_time_rate;
    float skill_overcharge_time_rate;
    float experience_amount;
    float physical_critical_hit;
    float magical_critical_hit;
    float stuck;
} DnfArtifactInfo;

/* CRUD Operations */
int DnfArtifactInfo_Add(DBConnectionManager* manager, const DnfArtifactInfo* record);
int DnfArtifactInfo_Get(DBConnectionManager* manager, int it_id, DnfArtifactInfo* record);
int DnfArtifactInfo_Update(DBConnectionManager* manager, const DnfArtifactInfo* record);
int DnfArtifactInfo_Delete(DBConnectionManager* manager, int it_id);
int DnfArtifactInfo_Exists(DBConnectionManager* manager, int it_id);
int DnfArtifactInfo_GetAll(DBConnectionManager* manager, DnfArtifactInfo* records, int max_count, int* actual_count);

#endif /* DNF_ARTIFACT_INFO_ORM_H */