#ifndef ONECLICK_CHARAC_AVATAR_MOD_ORM_H
#define ONECLICK_CHARAC_AVATAR_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    int ui_id_old;
    int slot_old;
    char reg_date_old[20];
    char expire_date_old[20];
    signed char ability_no_old;
    short color1_old;
    short color2_old;
    signed char trade_restrict_old;
    int ui_id_new;
    int slot_new;
    char reg_date_new[20];
    char expire_date_new[20];
    signed char ability_no_new;
    short color1_new;
    short color2_new;
    signed char trade_restrict_new;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} OneclickCharacAvatarMod;

/* CRUD Operations */
int OneclickCharacAvatarMod_Add(DBConnectionManager* manager, const OneclickCharacAvatarMod* record);
int OneclickCharacAvatarMod_Get(DBConnectionManager* manager, int id, OneclickCharacAvatarMod* record);
int OneclickCharacAvatarMod_Update(DBConnectionManager* manager, const OneclickCharacAvatarMod* record);
int OneclickCharacAvatarMod_Delete(DBConnectionManager* manager, int id);
int OneclickCharacAvatarMod_Exists(DBConnectionManager* manager, int id);
int OneclickCharacAvatarMod_GetAll(DBConnectionManager* manager, OneclickCharacAvatarMod* records, int max_count, int* actual_count);

#endif /* ONECLICK_CHARAC_AVATAR_MOD_ORM_H */