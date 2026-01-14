#ifndef CHARAC_HOUSING_TREE_MOD_ORM_H
#define CHARAC_HOUSING_TREE_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    int tree_id_old;
    int tree_id_new;
    char expire_date_old[20];
    char expire_date_new[20];
    short current_point_old;
    short current_point_new;
    short leaf_point_old;
    short leaf_point_new;
    short day_water_count_old;
    short day_water_count_new;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} CharacHousingTreeMod;

/* CRUD Operations */
int CharacHousingTreeMod_Add(DBConnectionManager* manager, const CharacHousingTreeMod* record);
int CharacHousingTreeMod_Get(DBConnectionManager* manager, int id, CharacHousingTreeMod* record);
int CharacHousingTreeMod_Update(DBConnectionManager* manager, const CharacHousingTreeMod* record);
int CharacHousingTreeMod_Delete(DBConnectionManager* manager, int id);
int CharacHousingTreeMod_Exists(DBConnectionManager* manager, int id);
int CharacHousingTreeMod_GetAll(DBConnectionManager* manager, CharacHousingTreeMod* records, int max_count, int* actual_count);

#endif /* CHARAC_HOUSING_TREE_MOD_ORM_H */