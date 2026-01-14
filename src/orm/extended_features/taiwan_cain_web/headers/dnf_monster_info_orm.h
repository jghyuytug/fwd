#ifndef DNF_MONSTER_INFO_ORM_H
#define DNF_MONSTER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    char mon_name_kr[121];
} DnfMonsterInfo;

/* CRUD Operations */
int DnfMonsterInfo_Add(DBConnectionManager* manager, const DnfMonsterInfo* record);
int DnfMonsterInfo_GetAll(DBConnectionManager* manager, DnfMonsterInfo* records, int max_count, int* actual_count);

#endif /* DNF_MONSTER_INFO_ORM_H */