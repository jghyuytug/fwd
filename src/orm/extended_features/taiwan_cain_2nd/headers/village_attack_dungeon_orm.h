#ifndef VILLAGE_ATTACK_DUNGEON_ORM_H
#define VILLAGE_ATTACK_DUNGEON_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int charac_no;
    signed char attack_count;
    signed char revenge_dungeon;
} VillageAttackDungeon;

/* CRUD Operations */
int VillageAttackDungeon_Add(DBConnectionManager* manager, const VillageAttackDungeon* record);
int VillageAttackDungeon_Get(DBConnectionManager* manager, const char* occ_date, int charac_no, VillageAttackDungeon* record);
int VillageAttackDungeon_Update(DBConnectionManager* manager, const VillageAttackDungeon* record);
int VillageAttackDungeon_Delete(DBConnectionManager* manager, const char* occ_date, int charac_no);
int VillageAttackDungeon_Exists(DBConnectionManager* manager, const char* occ_date, int charac_no);
int VillageAttackDungeon_GetAll(DBConnectionManager* manager, VillageAttackDungeon* records, int max_count, int* actual_count);

#endif /* VILLAGE_ATTACK_DUNGEON_ORM_H */