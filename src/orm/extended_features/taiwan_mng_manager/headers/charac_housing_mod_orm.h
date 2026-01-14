#ifndef CHARAC_HOUSING_MOD_ORM_H
#define CHARAC_HOUSING_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    signed char installed_old;
    signed char installed_new;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} CharacHousingMod;

/* CRUD Operations */
int CharacHousingMod_Add(DBConnectionManager* manager, const CharacHousingMod* record);
int CharacHousingMod_Get(DBConnectionManager* manager, int id, CharacHousingMod* record);
int CharacHousingMod_Update(DBConnectionManager* manager, const CharacHousingMod* record);
int CharacHousingMod_Delete(DBConnectionManager* manager, int id);
int CharacHousingMod_Exists(DBConnectionManager* manager, int id);
int CharacHousingMod_GetAll(DBConnectionManager* manager, CharacHousingMod* records, int max_count, int* actual_count);

#endif /* CHARAC_HOUSING_MOD_ORM_H */