#ifndef DNF_ITEM_INFO_ABILITY_ORM_H
#define DNF_ITEM_INFO_ABILITY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_no;
    int ability_no;
    char ability_name[101];
    signed char job;
    int skill_index;
    char ability_sign[3];
    short ability_add;
} DnfItemInfoAbility;

/* CRUD Operations */
int DnfItemInfoAbility_Add(DBConnectionManager* manager, const DnfItemInfoAbility* record);
int DnfItemInfoAbility_Get(DBConnectionManager* manager, int it_no, int ability_no, DnfItemInfoAbility* record);
int DnfItemInfoAbility_Update(DBConnectionManager* manager, const DnfItemInfoAbility* record);
int DnfItemInfoAbility_Delete(DBConnectionManager* manager, int it_no, int ability_no);
int DnfItemInfoAbility_Exists(DBConnectionManager* manager, int it_no, int ability_no);
int DnfItemInfoAbility_GetAll(DBConnectionManager* manager, DnfItemInfoAbility* records, int max_count, int* actual_count);

#endif /* DNF_ITEM_INFO_ABILITY_ORM_H */