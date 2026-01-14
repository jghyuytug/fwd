#ifndef TME_CHARAC_ORM_H
#define TME_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    char charac_name[11];
    signed char village;
    signed char job;
    signed char lev;
    int exp;
    signed char grow_type;
    signed char HP;
    short maxHP;
    short maxMP;
    short phy_attack;
    short phy_defense;
    short mag_attack;
    short mag_defense;
    int inven_weight;
    short hp_regen;
    short mp_regen;
    short move_speed;
    short attack_speed;
    short cast_speed;
    short hit_recovery;
    short jump;
    int charac_weight;
    short fatigue;
    short max_fatigue;
    short premium_fatigue;
    short max_premium_fatigue;
    char create_time[20];
    char last_play_time[20];
    int dungeon_clear_point;
    char delete_time[20];
    signed char delete_flag;
    int guild_id;
    signed char guild_right;
    signed char member_flag;
} TmeCharac;

/* CRUD Operations */
int TmeCharac_Add(DBConnectionManager* manager, const TmeCharac* record);
int TmeCharac_GetAll(DBConnectionManager* manager, TmeCharac* records, int max_count, int* actual_count);

#endif /* TME_CHARAC_ORM_H */