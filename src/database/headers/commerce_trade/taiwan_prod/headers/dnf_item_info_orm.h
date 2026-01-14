#ifndef DNF_ITEM_INFO_ORM_H
#define DNF_ITEM_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_no;
    char it_name[51];
    char it_eng_name[51];
    char it_explain[61];
    signed char master_type;
    short sub_type;
    char job[13];
    signed char class;
    char revert[6];
    signed char level;
    short skill;
    float create_ratio;
    signed char rarity;
    short weight;
    short price;
    short cash;
    short medal;
    short durability;
    short cooltime;
    short hp_max;
    short mp_max;
    short phy_att;
    short phy_def;
    short mag_att;
    short mag_def;
    short equip_phy_att;
    short equip_phy_def;
    short equip_mag_att;
    short equip_mag_def;
    signed char ref_fire;
    signed char ref_water;
    signed char ref_dark;
    signed char ref_light;
    signed char ref_all;
    signed char ref_slow;
    signed char ref_freeze;
    signed char ref_poison;
    signed char ref_stun;
    signed char ref_cus;
    signed char ref_blind;
    signed char ref_lite;
    signed char ref_ston;
    signed char ref_sleep;
    signed char ref_deekement;
    signed char ref_deadlystrike;
    signed char ref_bleeding;
    signed char ref_confuse;
    signed char ref_hold;
    signed char ref_all_stat;
    short ref_pierce;
    short ref_stuck;
    short inven_max;
    short hp_regenrate;
    short mp_regenrate;
    short mov_speed;
    short att_speed;
    short quest;
    short hit_recovery;
    short jump;
    char att_element;
    short att_active_status;
    float att_active_status_ratio;
    short att_active_status_pow;
    short att_backforce;
    short att_upforce;
    signed char att_hp_drain;
    signed char att_mp_drain;
    float criticalhit_rate;
    float stuck_rate;
    signed char att_defenseIgnore;
    char skill_levelup[26];
    char set_type;
    char url[65];
} DnfItemInfo;

/* CRUD Operations */
int DnfItemInfo_Add(DBConnectionManager* manager, const DnfItemInfo* record);
int DnfItemInfo_Get(DBConnectionManager* manager, int it_no, DnfItemInfo* record);
int DnfItemInfo_Update(DBConnectionManager* manager, const DnfItemInfo* record);
int DnfItemInfo_Delete(DBConnectionManager* manager, int it_no);
int DnfItemInfo_Exists(DBConnectionManager* manager, int it_no);
int DnfItemInfo_GetAll(DBConnectionManager* manager, DnfItemInfo* records, int max_count, int* actual_count);

#endif /* DNF_ITEM_INFO_ORM_H */