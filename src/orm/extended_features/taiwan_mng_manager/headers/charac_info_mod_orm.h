#ifndef CHARAC_INFO_MOD_ORM_H
#define CHARAC_INFO_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    signed char lev_old;
    int exp_old;
    short fatigue_old;
    short max_fatigue_old;
    short premium_fatigue_old;
    short max_premium_fatigue_old;
    int money_old;
    int coin_old;
    int event_coin_old;
    int pay_coin_old;
    signed char lev_new;
    int exp_new;
    short fatigue_new;
    short max_fatigue_new;
    short premium_fatigue_new;
    short max_premium_fatigue_new;
    int money_new;
    int coin_new;
    int event_coin_new;
    int pay_coin_new;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
    int win_point_old;
    int pvp_point_old;
    int pvp_grade_old;
    int win_old;
    int lose_old;
    int help_abuse_ratio_old;
    int help_abuse_exp_old;
    short power_war_point_old;
    int win_point_new;
    int pvp_point_new;
    int pvp_grade_new;
    int win_new;
    int lose_new;
    int help_abuse_ratio_new;
    int help_abuse_exp_new;
    short power_war_point_new;
} CharacInfoMod;

/* CRUD Operations */
int CharacInfoMod_Add(DBConnectionManager* manager, const CharacInfoMod* record);
int CharacInfoMod_Get(DBConnectionManager* manager, int id, CharacInfoMod* record);
int CharacInfoMod_Update(DBConnectionManager* manager, const CharacInfoMod* record);
int CharacInfoMod_Delete(DBConnectionManager* manager, int id);
int CharacInfoMod_Exists(DBConnectionManager* manager, int id);
int CharacInfoMod_GetAll(DBConnectionManager* manager, CharacInfoMod* records, int max_count, int* actual_count);

#endif /* CHARAC_INFO_MOD_ORM_H */