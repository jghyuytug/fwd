#ifndef PROD_JOB_ABILITY_INFO_ORM_H
#define PROD_JOB_ABILITY_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char job;
    signed char grow_type;
    int skill_index;
    char skill_name[101];
} ProdJobAbilityInfo;

/* CRUD Operations */
int ProdJobAbilityInfo_Add(DBConnectionManager* manager, const ProdJobAbilityInfo* record);
int ProdJobAbilityInfo_Get(DBConnectionManager* manager, signed char job, signed char grow_type, int skill_index, ProdJobAbilityInfo* record);
int ProdJobAbilityInfo_Update(DBConnectionManager* manager, const ProdJobAbilityInfo* record);
int ProdJobAbilityInfo_Delete(DBConnectionManager* manager, signed char job, signed char grow_type, int skill_index);
int ProdJobAbilityInfo_Exists(DBConnectionManager* manager, signed char job, signed char grow_type, int skill_index);
int ProdJobAbilityInfo_GetAll(DBConnectionManager* manager, ProdJobAbilityInfo* records, int max_count, int* actual_count);

#endif /* PROD_JOB_ABILITY_INFO_ORM_H */