#ifndef AVATAR_MOD_ORM_H
#define AVATAR_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    int avatar_id;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char reg_date[20];
    char expire_date[20];
    signed char ability_no;
    char memo[256];
} AvatarMod;

/* CRUD Operations */
int AvatarMod_Add(DBConnectionManager* manager, const AvatarMod* record);
int AvatarMod_Get(DBConnectionManager* manager, int id, AvatarMod* record);
int AvatarMod_Update(DBConnectionManager* manager, const AvatarMod* record);
int AvatarMod_Delete(DBConnectionManager* manager, int id);
int AvatarMod_Exists(DBConnectionManager* manager, int id);
int AvatarMod_GetAll(DBConnectionManager* manager, AvatarMod* records, int max_count, int* actual_count);

#endif /* AVATAR_MOD_ORM_H */