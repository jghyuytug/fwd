#include "dnf_old_equip_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfOldEquipInfo_Add(DBConnectionManager* manager, const DnfOldEquipInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_old_equip_info (it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->it_id, record->hp_max, record->mp_max, record->phy_att, record->phy_def, record->mag_att, record->mag_def, record->equip_phy_att, record->equip_phy_def, record->equip_mag_att, record->equip_mag_def, record->ref_fire, record->ref_water, record->ref_dark, record->ref_light, record->ref_all_elements, record->ref_slow, record->ref_freeze, record->ref_poison, record->ref_stun, record->ref_curse, record->ref_blind, record->ref_lightning, record->ref_stone, record->ref_sleep, record->ref_burn, record->ref_weapon_break, record->ref_bleeding, record->ref_pierce, record->ref_stuck, record->ref_confuse, record->ref_hold, record->ref_armor_break, record->ref_all_state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfOldEquipInfo_Get(DBConnectionManager* manager, int it_id, DnfOldEquipInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[34];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state FROM dnf_old_equip_info WHERE it_id = %d",
        it_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfOldEquipInfo));
    record->it_id = row[0] ? atoi(row[0]) : 0;
    record->hp_max = row[1] ? atoi(row[1]) : 0;
    record->mp_max = row[2] ? atoi(row[2]) : 0;
    record->phy_att = row[3] ? atoi(row[3]) : 0;
    record->phy_def = row[4] ? atoi(row[4]) : 0;
    record->mag_att = row[5] ? atoi(row[5]) : 0;
    record->mag_def = row[6] ? atoi(row[6]) : 0;
    record->equip_phy_att = row[7] ? atoi(row[7]) : 0;
    record->equip_phy_def = row[8] ? atoi(row[8]) : 0;
    record->equip_mag_att = row[9] ? atoi(row[9]) : 0;
    record->equip_mag_def = row[10] ? atoi(row[10]) : 0;
    record->ref_fire = row[11] ? atoi(row[11]) : 0;
    record->ref_water = row[12] ? atoi(row[12]) : 0;
    record->ref_dark = row[13] ? atoi(row[13]) : 0;
    record->ref_light = row[14] ? atoi(row[14]) : 0;
    record->ref_all_elements = row[15] ? atoi(row[15]) : 0;
    record->ref_slow = row[16] ? atoi(row[16]) : 0;
    record->ref_freeze = row[17] ? atoi(row[17]) : 0;
    record->ref_poison = row[18] ? atoi(row[18]) : 0;
    record->ref_stun = row[19] ? atoi(row[19]) : 0;
    record->ref_curse = row[20] ? atoi(row[20]) : 0;
    record->ref_blind = row[21] ? atoi(row[21]) : 0;
    record->ref_lightning = row[22] ? atoi(row[22]) : 0;
    record->ref_stone = row[23] ? atoi(row[23]) : 0;
    record->ref_sleep = row[24] ? atoi(row[24]) : 0;
    record->ref_burn = row[25] ? atoi(row[25]) : 0;
    record->ref_weapon_break = row[26] ? atoi(row[26]) : 0;
    record->ref_bleeding = row[27] ? atoi(row[27]) : 0;
    record->ref_pierce = row[28] ? atoi(row[28]) : 0;
    record->ref_stuck = row[29] ? atoi(row[29]) : 0;
    record->ref_confuse = row[30] ? atoi(row[30]) : 0;
    record->ref_hold = row[31] ? atoi(row[31]) : 0;
    record->ref_armor_break = row[32] ? atoi(row[32]) : 0;
    record->ref_all_state = row[33] ? atoi(row[33]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfOldEquipInfo_GetAll(DBConnectionManager* manager, DnfOldEquipInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[34];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state FROM dnf_old_equip_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfOldEquipInfo));
        records[count].it_id = row[0] ? atoi(row[0]) : 0;
        records[count].hp_max = row[1] ? atoi(row[1]) : 0;
        records[count].mp_max = row[2] ? atoi(row[2]) : 0;
        records[count].phy_att = row[3] ? atoi(row[3]) : 0;
        records[count].phy_def = row[4] ? atoi(row[4]) : 0;
        records[count].mag_att = row[5] ? atoi(row[5]) : 0;
        records[count].mag_def = row[6] ? atoi(row[6]) : 0;
        records[count].equip_phy_att = row[7] ? atoi(row[7]) : 0;
        records[count].equip_phy_def = row[8] ? atoi(row[8]) : 0;
        records[count].equip_mag_att = row[9] ? atoi(row[9]) : 0;
        records[count].equip_mag_def = row[10] ? atoi(row[10]) : 0;
        records[count].ref_fire = row[11] ? atoi(row[11]) : 0;
        records[count].ref_water = row[12] ? atoi(row[12]) : 0;
        records[count].ref_dark = row[13] ? atoi(row[13]) : 0;
        records[count].ref_light = row[14] ? atoi(row[14]) : 0;
        records[count].ref_all_elements = row[15] ? atoi(row[15]) : 0;
        records[count].ref_slow = row[16] ? atoi(row[16]) : 0;
        records[count].ref_freeze = row[17] ? atoi(row[17]) : 0;
        records[count].ref_poison = row[18] ? atoi(row[18]) : 0;
        records[count].ref_stun = row[19] ? atoi(row[19]) : 0;
        records[count].ref_curse = row[20] ? atoi(row[20]) : 0;
        records[count].ref_blind = row[21] ? atoi(row[21]) : 0;
        records[count].ref_lightning = row[22] ? atoi(row[22]) : 0;
        records[count].ref_stone = row[23] ? atoi(row[23]) : 0;
        records[count].ref_sleep = row[24] ? atoi(row[24]) : 0;
        records[count].ref_burn = row[25] ? atoi(row[25]) : 0;
        records[count].ref_weapon_break = row[26] ? atoi(row[26]) : 0;
        records[count].ref_bleeding = row[27] ? atoi(row[27]) : 0;
        records[count].ref_pierce = row[28] ? atoi(row[28]) : 0;
        records[count].ref_stuck = row[29] ? atoi(row[29]) : 0;
        records[count].ref_confuse = row[30] ? atoi(row[30]) : 0;
        records[count].ref_hold = row[31] ? atoi(row[31]) : 0;
        records[count].ref_armor_break = row[32] ? atoi(row[32]) : 0;
        records[count].ref_all_state = row[33] ? atoi(row[33]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
