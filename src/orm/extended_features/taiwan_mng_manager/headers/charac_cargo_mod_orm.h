#ifndef CHARAC_CARGO_MOD_ORM_H
#define CHARAC_CARGO_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    int cargo_capa_old;
    int cargo_capa_new;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} CharacCargoMod;

/* CRUD Operations */
int CharacCargoMod_Add(DBConnectionManager* manager, const CharacCargoMod* record);
int CharacCargoMod_Get(DBConnectionManager* manager, int id, CharacCargoMod* record);
int CharacCargoMod_Update(DBConnectionManager* manager, const CharacCargoMod* record);
int CharacCargoMod_Delete(DBConnectionManager* manager, int id);
int CharacCargoMod_Exists(DBConnectionManager* manager, int id);
int CharacCargoMod_GetAll(DBConnectionManager* manager, CharacCargoMod* records, int max_count, int* actual_count);

#endif /* CHARAC_CARGO_MOD_ORM_H */