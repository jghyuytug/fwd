#ifndef CHARAC_CREATURE_INVEN_MOD_ORM_H
#define CHARAC_CREATURE_INVEN_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} CharacCreatureInvenMod;

/* CRUD Operations */
int CharacCreatureInvenMod_Add(DBConnectionManager* manager, const CharacCreatureInvenMod* record);
int CharacCreatureInvenMod_Get(DBConnectionManager* manager, int id, CharacCreatureInvenMod* record);
int CharacCreatureInvenMod_Update(DBConnectionManager* manager, const CharacCreatureInvenMod* record);
int CharacCreatureInvenMod_Delete(DBConnectionManager* manager, int id);
int CharacCreatureInvenMod_Exists(DBConnectionManager* manager, int id);
int CharacCreatureInvenMod_GetAll(DBConnectionManager* manager, CharacCreatureInvenMod* records, int max_count, int* actual_count);

#endif /* CHARAC_CREATURE_INVEN_MOD_ORM_H */