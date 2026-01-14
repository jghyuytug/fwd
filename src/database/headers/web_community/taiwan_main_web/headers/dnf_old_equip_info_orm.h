#ifndef DNF_OLD_EQUIP_INFO_ORM_H
#define DNF_OLD_EQUIP_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_id;
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
    short ref_fire;
    short ref_water;
    short ref_dark;
    short ref_light;
    short ref_all_elements;
    short ref_slow;
    short ref_freeze;
    short ref_poison;
    short ref_stun;
    short ref_curse;
    short ref_blind;
    short ref_lightning;
    short ref_stone;
    short ref_sleep;
    short ref_burn;
    short ref_weapon_break;
    short ref_bleeding;
    short ref_pierce;
    short ref_stuck;
    short ref_confuse;
    short ref_hold;
    short ref_armor_break;
    short ref_all_state;
} DnfOldEquipInfo;

/* CRUD Operations */
int DnfOldEquipInfo_Add(DBConnectionManager* manager, const DnfOldEquipInfo* record);
int DnfOldEquipInfo_Get(DBConnectionManager* manager, int it_id, DnfOldEquipInfo* record);
int DnfOldEquipInfo_Update(DBConnectionManager* manager, const DnfOldEquipInfo* record);
int DnfOldEquipInfo_Delete(DBConnectionManager* manager, int it_id);
int DnfOldEquipInfo_Exists(DBConnectionManager* manager, int it_id);
int DnfOldEquipInfo_GetAll(DBConnectionManager* manager, DnfOldEquipInfo* records, int max_count, int* actual_count);

#endif /* DNF_OLD_EQUIP_INFO_ORM_H */