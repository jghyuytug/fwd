#ifndef PROD_ABILITY_INFO_ORM_H
#define PROD_ABILITY_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ipg_no;
    int it_no;
    signed char job;
    signed char grow_type;
    int ability_no;
    short rate;
} ProdAbilityInfo;

/* CRUD Operations */
int ProdAbilityInfo_Add(DBConnectionManager* manager, const ProdAbilityInfo* record);
int ProdAbilityInfo_Get(DBConnectionManager* manager, int ipg_no, signed char job, signed char grow_type, int ability_no, ProdAbilityInfo* record);
int ProdAbilityInfo_Update(DBConnectionManager* manager, const ProdAbilityInfo* record);
int ProdAbilityInfo_Delete(DBConnectionManager* manager, int ipg_no, signed char job, signed char grow_type, int ability_no);
int ProdAbilityInfo_Exists(DBConnectionManager* manager, int ipg_no, signed char job, signed char grow_type, int ability_no);
int ProdAbilityInfo_GetAll(DBConnectionManager* manager, ProdAbilityInfo* records, int max_count, int* actual_count);

#endif /* PROD_ABILITY_INFO_ORM_H */