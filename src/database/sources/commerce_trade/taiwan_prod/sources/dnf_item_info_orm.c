#include "dnf_item_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfItemInfo_Add(DBConnectionManager* manager, const DnfItemInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_item_info (it_no, it_name, it_eng_name, it_explain, master_type, sub_type, job, class, revert, level, skill, create_ratio, rarity, weight, price, cash, medal, durability, cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, hit_recovery, jump, att_element, att_active_status, att_active_status_ratio, att_active_status_pow, att_backforce, att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, stuck_rate, att_defenseIgnore, skill_levelup, set_type, url) "
        "VALUES (%d, '%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, %f, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, %d, %d, %f, %f, %d, '%s', %d, '%s')",
        record->it_no, record->it_name, record->it_eng_name, record->it_explain, record->master_type, record->sub_type, record->job, record->class, record->revert, record->level, record->skill, record->create_ratio, record->rarity, record->weight, record->price, record->cash, record->medal, record->durability, record->cooltime, record->hp_max, record->mp_max, record->phy_att, record->phy_def, record->mag_att, record->mag_def, record->equip_phy_att, record->equip_phy_def, record->equip_mag_att, record->equip_mag_def, record->ref_fire, record->ref_water, record->ref_dark, record->ref_light, record->ref_all, record->ref_slow, record->ref_freeze, record->ref_poison, record->ref_stun, record->ref_cus, record->ref_blind, record->ref_lite, record->ref_ston, record->ref_sleep, record->ref_deekement, record->ref_deadlystrike, record->ref_bleeding, record->ref_confuse, record->ref_hold, record->ref_all_stat, record->ref_pierce, record->ref_stuck, record->inven_max, record->hp_regenrate, record->mp_regenrate, record->mov_speed, record->att_speed, record->quest, record->hit_recovery, record->jump, record->att_element, record->att_active_status, record->att_active_status_ratio, record->att_active_status_pow, record->att_backforce, record->att_upforce, record->att_hp_drain, record->att_mp_drain, record->criticalhit_rate, record->stuck_rate, record->att_defenseIgnore, record->skill_levelup, record->set_type, record->url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfo_Get(DBConnectionManager* manager, int it_no, DnfItemInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[73];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, it_name, it_eng_name, it_explain, master_type, sub_type, job, class, revert, level, skill, create_ratio, rarity, weight, price, cash, medal, durability, cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, hit_recovery, jump, att_element, att_active_status, att_active_status_ratio, att_active_status_pow, att_backforce, att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, stuck_rate, att_defenseIgnore, skill_levelup, set_type, url FROM dnf_item_info WHERE it_no = %d",
        it_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfItemInfo));
    record->it_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->it_name, row[1], sizeof(record->it_name) - 1);
    if (row[2]) strncpy(record->it_eng_name, row[2], sizeof(record->it_eng_name) - 1);
    if (row[3]) strncpy(record->it_explain, row[3], sizeof(record->it_explain) - 1);
    record->master_type = row[4] ? atoi(row[4]) : 0;
    record->sub_type = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->job, row[6], sizeof(record->job) - 1);
    record->class = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->revert, row[8], sizeof(record->revert) - 1);
    record->level = row[9] ? atoi(row[9]) : 0;
    record->skill = row[10] ? atoi(row[10]) : 0;
    record->create_ratio = row[11] ? atof(row[11]) : 0;
    record->rarity = row[12] ? atoi(row[12]) : 0;
    record->weight = row[13] ? atoi(row[13]) : 0;
    record->price = row[14] ? atoi(row[14]) : 0;
    record->cash = row[15] ? atoi(row[15]) : 0;
    record->medal = row[16] ? atoi(row[16]) : 0;
    record->durability = row[17] ? atoi(row[17]) : 0;
    record->cooltime = row[18] ? atoi(row[18]) : 0;
    record->hp_max = row[19] ? atoi(row[19]) : 0;
    record->mp_max = row[20] ? atoi(row[20]) : 0;
    record->phy_att = row[21] ? atoi(row[21]) : 0;
    record->phy_def = row[22] ? atoi(row[22]) : 0;
    record->mag_att = row[23] ? atoi(row[23]) : 0;
    record->mag_def = row[24] ? atoi(row[24]) : 0;
    record->equip_phy_att = row[25] ? atoi(row[25]) : 0;
    record->equip_phy_def = row[26] ? atoi(row[26]) : 0;
    record->equip_mag_att = row[27] ? atoi(row[27]) : 0;
    record->equip_mag_def = row[28] ? atoi(row[28]) : 0;
    record->ref_fire = row[29] ? atoi(row[29]) : 0;
    record->ref_water = row[30] ? atoi(row[30]) : 0;
    record->ref_dark = row[31] ? atoi(row[31]) : 0;
    record->ref_light = row[32] ? atoi(row[32]) : 0;
    record->ref_all = row[33] ? atoi(row[33]) : 0;
    record->ref_slow = row[34] ? atoi(row[34]) : 0;
    record->ref_freeze = row[35] ? atoi(row[35]) : 0;
    record->ref_poison = row[36] ? atoi(row[36]) : 0;
    record->ref_stun = row[37] ? atoi(row[37]) : 0;
    record->ref_cus = row[38] ? atoi(row[38]) : 0;
    record->ref_blind = row[39] ? atoi(row[39]) : 0;
    record->ref_lite = row[40] ? atoi(row[40]) : 0;
    record->ref_ston = row[41] ? atoi(row[41]) : 0;
    record->ref_sleep = row[42] ? atoi(row[42]) : 0;
    record->ref_deekement = row[43] ? atoi(row[43]) : 0;
    record->ref_deadlystrike = row[44] ? atoi(row[44]) : 0;
    record->ref_bleeding = row[45] ? atoi(row[45]) : 0;
    record->ref_confuse = row[46] ? atoi(row[46]) : 0;
    record->ref_hold = row[47] ? atoi(row[47]) : 0;
    record->ref_all_stat = row[48] ? atoi(row[48]) : 0;
    record->ref_pierce = row[49] ? atoi(row[49]) : 0;
    record->ref_stuck = row[50] ? atoi(row[50]) : 0;
    record->inven_max = row[51] ? atoi(row[51]) : 0;
    record->hp_regenrate = row[52] ? atoi(row[52]) : 0;
    record->mp_regenrate = row[53] ? atoi(row[53]) : 0;
    record->mov_speed = row[54] ? atoi(row[54]) : 0;
    record->att_speed = row[55] ? atoi(row[55]) : 0;
    record->quest = row[56] ? atoi(row[56]) : 0;
    record->hit_recovery = row[57] ? atoi(row[57]) : 0;
    record->jump = row[58] ? atoi(row[58]) : 0;
    record->att_element = row[59] ? row[59][0] : '\0';
    record->att_active_status = row[60] ? atoi(row[60]) : 0;
    record->att_active_status_ratio = row[61] ? atof(row[61]) : 0;
    record->att_active_status_pow = row[62] ? atoi(row[62]) : 0;
    record->att_backforce = row[63] ? atoi(row[63]) : 0;
    record->att_upforce = row[64] ? atoi(row[64]) : 0;
    record->att_hp_drain = row[65] ? atoi(row[65]) : 0;
    record->att_mp_drain = row[66] ? atoi(row[66]) : 0;
    record->criticalhit_rate = row[67] ? atof(row[67]) : 0;
    record->stuck_rate = row[68] ? atof(row[68]) : 0;
    record->att_defenseIgnore = row[69] ? atoi(row[69]) : 0;
    if (row[70]) strncpy(record->skill_levelup, row[70], sizeof(record->skill_levelup) - 1);
    record->set_type = row[71] ? row[71][0] : '\0';
    if (row[72]) strncpy(record->url, row[72], sizeof(record->url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfo_GetAll(DBConnectionManager* manager, DnfItemInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[73];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, it_name, it_eng_name, it_explain, master_type, sub_type, job, class, revert, level, skill, create_ratio, rarity, weight, price, cash, medal, durability, cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, hit_recovery, jump, att_element, att_active_status, att_active_status_ratio, att_active_status_pow, att_backforce, att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, stuck_rate, att_defenseIgnore, skill_levelup, set_type, url FROM dnf_item_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfItemInfo));
        records[count].it_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].it_name, row[1], sizeof(records[count].it_name) - 1);
        if (row[2]) strncpy(records[count].it_eng_name, row[2], sizeof(records[count].it_eng_name) - 1);
        if (row[3]) strncpy(records[count].it_explain, row[3], sizeof(records[count].it_explain) - 1);
        records[count].master_type = row[4] ? atoi(row[4]) : 0;
        records[count].sub_type = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].job, row[6], sizeof(records[count].job) - 1);
        records[count].class = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].revert, row[8], sizeof(records[count].revert) - 1);
        records[count].level = row[9] ? atoi(row[9]) : 0;
        records[count].skill = row[10] ? atoi(row[10]) : 0;
        records[count].create_ratio = row[11] ? atof(row[11]) : 0;
        records[count].rarity = row[12] ? atoi(row[12]) : 0;
        records[count].weight = row[13] ? atoi(row[13]) : 0;
        records[count].price = row[14] ? atoi(row[14]) : 0;
        records[count].cash = row[15] ? atoi(row[15]) : 0;
        records[count].medal = row[16] ? atoi(row[16]) : 0;
        records[count].durability = row[17] ? atoi(row[17]) : 0;
        records[count].cooltime = row[18] ? atoi(row[18]) : 0;
        records[count].hp_max = row[19] ? atoi(row[19]) : 0;
        records[count].mp_max = row[20] ? atoi(row[20]) : 0;
        records[count].phy_att = row[21] ? atoi(row[21]) : 0;
        records[count].phy_def = row[22] ? atoi(row[22]) : 0;
        records[count].mag_att = row[23] ? atoi(row[23]) : 0;
        records[count].mag_def = row[24] ? atoi(row[24]) : 0;
        records[count].equip_phy_att = row[25] ? atoi(row[25]) : 0;
        records[count].equip_phy_def = row[26] ? atoi(row[26]) : 0;
        records[count].equip_mag_att = row[27] ? atoi(row[27]) : 0;
        records[count].equip_mag_def = row[28] ? atoi(row[28]) : 0;
        records[count].ref_fire = row[29] ? atoi(row[29]) : 0;
        records[count].ref_water = row[30] ? atoi(row[30]) : 0;
        records[count].ref_dark = row[31] ? atoi(row[31]) : 0;
        records[count].ref_light = row[32] ? atoi(row[32]) : 0;
        records[count].ref_all = row[33] ? atoi(row[33]) : 0;
        records[count].ref_slow = row[34] ? atoi(row[34]) : 0;
        records[count].ref_freeze = row[35] ? atoi(row[35]) : 0;
        records[count].ref_poison = row[36] ? atoi(row[36]) : 0;
        records[count].ref_stun = row[37] ? atoi(row[37]) : 0;
        records[count].ref_cus = row[38] ? atoi(row[38]) : 0;
        records[count].ref_blind = row[39] ? atoi(row[39]) : 0;
        records[count].ref_lite = row[40] ? atoi(row[40]) : 0;
        records[count].ref_ston = row[41] ? atoi(row[41]) : 0;
        records[count].ref_sleep = row[42] ? atoi(row[42]) : 0;
        records[count].ref_deekement = row[43] ? atoi(row[43]) : 0;
        records[count].ref_deadlystrike = row[44] ? atoi(row[44]) : 0;
        records[count].ref_bleeding = row[45] ? atoi(row[45]) : 0;
        records[count].ref_confuse = row[46] ? atoi(row[46]) : 0;
        records[count].ref_hold = row[47] ? atoi(row[47]) : 0;
        records[count].ref_all_stat = row[48] ? atoi(row[48]) : 0;
        records[count].ref_pierce = row[49] ? atoi(row[49]) : 0;
        records[count].ref_stuck = row[50] ? atoi(row[50]) : 0;
        records[count].inven_max = row[51] ? atoi(row[51]) : 0;
        records[count].hp_regenrate = row[52] ? atoi(row[52]) : 0;
        records[count].mp_regenrate = row[53] ? atoi(row[53]) : 0;
        records[count].mov_speed = row[54] ? atoi(row[54]) : 0;
        records[count].att_speed = row[55] ? atoi(row[55]) : 0;
        records[count].quest = row[56] ? atoi(row[56]) : 0;
        records[count].hit_recovery = row[57] ? atoi(row[57]) : 0;
        records[count].jump = row[58] ? atoi(row[58]) : 0;
        records[count].att_element = row[59] ? row[59][0] : '\0';
        records[count].att_active_status = row[60] ? atoi(row[60]) : 0;
        records[count].att_active_status_ratio = row[61] ? atof(row[61]) : 0;
        records[count].att_active_status_pow = row[62] ? atoi(row[62]) : 0;
        records[count].att_backforce = row[63] ? atoi(row[63]) : 0;
        records[count].att_upforce = row[64] ? atoi(row[64]) : 0;
        records[count].att_hp_drain = row[65] ? atoi(row[65]) : 0;
        records[count].att_mp_drain = row[66] ? atoi(row[66]) : 0;
        records[count].criticalhit_rate = row[67] ? atof(row[67]) : 0;
        records[count].stuck_rate = row[68] ? atof(row[68]) : 0;
        records[count].att_defenseIgnore = row[69] ? atoi(row[69]) : 0;
        if (row[70]) strncpy(records[count].skill_levelup, row[70], sizeof(records[count].skill_levelup) - 1);
        records[count].set_type = row[71] ? row[71][0] : '\0';
        if (row[72]) strncpy(records[count].url, row[72], sizeof(records[count].url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
