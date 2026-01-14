#ifndef CREATURE_ITEMS_DEL_ORM_H
#define CREATURE_ITEMS_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
    int ui_id;
    int charac_no;
    signed char slot;
    int it_id;
    char reg_date[20];
    char name[13];
    int stomach;
    int exp;
    signed char endurance;
    signed char creature_type;
    signed char no_charge;
    signed char stat;
    signed char item_lock_key;
    char ipg_agency_no[33];
    char expire_date[20];
    char delete_date[20];
} CreatureItemsDel;

/* CRUD Operations */
int CreatureItemsDel_Add(DBConnectionManager* manager, const CreatureItemsDel* record);
int CreatureItemsDel_Get(DBConnectionManager* manager, const char* sdate, int ui_id, CreatureItemsDel* record);
int CreatureItemsDel_Update(DBConnectionManager* manager, const CreatureItemsDel* record);
int CreatureItemsDel_Delete(DBConnectionManager* manager, const char* sdate, int ui_id);
int CreatureItemsDel_Exists(DBConnectionManager* manager, const char* sdate, int ui_id);
int CreatureItemsDel_GetAll(DBConnectionManager* manager, CreatureItemsDel* records, int max_count, int* actual_count);

#endif /* CREATURE_ITEMS_DEL_ORM_H */