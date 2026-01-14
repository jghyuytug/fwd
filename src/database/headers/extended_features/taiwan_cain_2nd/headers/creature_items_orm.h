#ifndef CREATURE_ITEMS_ORM_H
#define CREATURE_ITEMS_ORM_H

#include "db_connection_manager.h"

typedef struct {
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
} CreatureItems;

/* CRUD Operations */
int CreatureItems_Add(DBConnectionManager* manager, const CreatureItems* record);
int CreatureItems_Get(DBConnectionManager* manager, int ui_id, CreatureItems* record);
int CreatureItems_Update(DBConnectionManager* manager, const CreatureItems* record);
int CreatureItems_Delete(DBConnectionManager* manager, int ui_id);
int CreatureItems_Exists(DBConnectionManager* manager, int ui_id);
int CreatureItems_GetAll(DBConnectionManager* manager, CreatureItems* records, int max_count, int* actual_count);

#endif /* CREATURE_ITEMS_ORM_H */