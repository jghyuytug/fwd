#ifndef CHARAC_AVATAR_MOD_ORM_H
#define CHARAC_AVATAR_MOD_ORM_H

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
    char jewel_socket_old[31];
    int ui_id_new;
    int slot_new;
    char reg_date_new[20];
    char expire_date_new[20];
    signed char ability_no_new;
    char jewel_socket_new[31];
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
    short hidden_option_old;
    short emblem_endurance_old;
    short hidden_option_new;
    short emblem_endurance_new;
    short color1_old;
    short color2_old;
    short color1_new;
    short color2_new;
    int trade_restrict_old;
    int trade_restrict_new;
} CharacAvatarMod;

/* CRUD Operations */
int CharacAvatarMod_Add(DBConnectionManager* manager, const CharacAvatarMod* record);
int CharacAvatarMod_Get(DBConnectionManager* manager, int id, CharacAvatarMod* record);
int CharacAvatarMod_Update(DBConnectionManager* manager, const CharacAvatarMod* record);
int CharacAvatarMod_Delete(DBConnectionManager* manager, int id);
int CharacAvatarMod_Exists(DBConnectionManager* manager, int id);
int CharacAvatarMod_GetAll(DBConnectionManager* manager, CharacAvatarMod* records, int max_count, int* actual_count);

#endif /* CHARAC_AVATAR_MOD_ORM_H */