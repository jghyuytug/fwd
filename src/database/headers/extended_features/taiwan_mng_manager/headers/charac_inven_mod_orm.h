#ifndef CHARAC_INVEN_MOD_ORM_H
#define CHARAC_INVEN_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
    int inventory_capacity_old;
    int inventory_capacity_new;
} CharacInvenMod;

/* CRUD Operations */
int CharacInvenMod_Add(DBConnectionManager* manager, const CharacInvenMod* record);
int CharacInvenMod_Get(DBConnectionManager* manager, int id, CharacInvenMod* record);
int CharacInvenMod_Update(DBConnectionManager* manager, const CharacInvenMod* record);
int CharacInvenMod_Delete(DBConnectionManager* manager, int id);
int CharacInvenMod_Exists(DBConnectionManager* manager, int id);
int CharacInvenMod_GetAll(DBConnectionManager* manager, CharacInvenMod* records, int max_count, int* actual_count);

#endif /* CHARAC_INVEN_MOD_ORM_H */