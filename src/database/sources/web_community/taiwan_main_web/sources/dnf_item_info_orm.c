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
        "INSERT INTO dnf_item_info (it_no, it_name, it_eng_name, it_explain, master_type, sub_type, job, class, revert, level, skill, create_ratio, rarity, weight, price, cash, medal, durability, cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, hit_recovery, jump, att_element, att_active_status, att_active_status_ratio, att_active_status_pow, att_backforce, att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, stuck_rate, att_defenseIgnore, skill_levelup, set_type, url, jewel_type, detail_explain, flavor_text, anti_evil, value, required_skill, need_material, physical_absolute_damage, physical_damage_reduce, physical_absolute_defense, magical_absolute_damage, magical_damage_reduce, magical_absolute_defense, fire_attack, water_attack, dark_attack, light_attack, weapon_break_resistance, armor_break_resistance, all_activestatus_resistance, rigidity, item_aura, magical_critical_hit, set_name, set_item, fullset_basic_explain, fullset_detail_explain, parameter_basic_explain, parameter_detail_explain, part_set_index, skill_data_up, hide_equipment, db_piece_count, set_item_master, it_set_no, ani_variation, ani_variation_expand, ani_variation2, ani_variation_expand2, hide_growtype_avatar, room_list_move_speed_rate, icon_mark_number, extra_icon_idx_list, hp_max_rate, mp_max_rate, all_attack) "
        "VALUES (%d, '%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, %f, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, %d, %d, %f, %f, %d, '%s', %d, '%s', '%s', '%s', '%s', %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %f, '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', %d, %d, %d, '%s', '%s', '%s', '%s', '%s', %f, %d, '%s', %f, %f, %d)",
        record->it_no, record->it_name, record->it_eng_name, record->it_explain, record->master_type, record->sub_type, record->job, record->class, record->revert, record->level, record->skill, record->create_ratio, record->rarity, record->weight, record->price, record->cash, record->medal, record->durability, record->cooltime, record->hp_max, record->mp_max, record->phy_att, record->phy_def, record->mag_att, record->mag_def, record->equip_phy_att, record->equip_phy_def, record->equip_mag_att, record->equip_mag_def, record->ref_fire, record->ref_water, record->ref_dark, record->ref_light, record->ref_all, record->ref_slow, record->ref_freeze, record->ref_poison, record->ref_stun, record->ref_cus, record->ref_blind, record->ref_lite, record->ref_ston, record->ref_sleep, record->ref_deekement, record->ref_deadlystrike, record->ref_bleeding, record->ref_confuse, record->ref_hold, record->ref_all_stat, record->ref_pierce, record->ref_stuck, record->inven_max, record->hp_regenrate, record->mp_regenrate, record->mov_speed, record->att_speed, record->quest, record->hit_recovery, record->jump, record->att_element, record->att_active_status, record->att_active_status_ratio, record->att_active_status_pow, record->att_backforce, record->att_upforce, record->att_hp_drain, record->att_mp_drain, record->criticalhit_rate, record->stuck_rate, record->att_defenseIgnore, record->skill_levelup, record->set_type, record->url, record->jewel_type, record->detail_explain, record->flavor_text, record->anti_evil, record->value, record->required_skill, record->need_material, record->physical_absolute_damage, record->physical_damage_reduce, record->physical_absolute_defense, record->magical_absolute_damage, record->magical_damage_reduce, record->magical_absolute_defense, record->fire_attack, record->water_attack, record->dark_attack, record->light_attack, record->weapon_break_resistance, record->armor_break_resistance, record->all_activestatus_resistance, record->rigidity, record->item_aura, record->magical_critical_hit, record->set_name, record->set_item, record->fullset_basic_explain, record->fullset_detail_explain, record->parameter_basic_explain, record->parameter_detail_explain, record->part_set_index, record->skill_data_up, record->hide_equipment, record->db_piece_count, record->set_item_master, record->it_set_no, record->ani_variation, record->ani_variation_expand, record->ani_variation2, record->ani_variation_expand2, record->hide_growtype_avatar, record->room_list_move_speed_rate, record->icon_mark_number, record->extra_icon_idx_list, record->hp_max_rate, record->mp_max_rate, record->all_attack);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfo_Get(DBConnectionManager* manager, int it_no, int it_set_no, signed char db_piece_count, DnfItemInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[119];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, it_name, it_eng_name, it_explain, master_type, sub_type, job, class, revert, level, skill, create_ratio, rarity, weight, price, cash, medal, durability, cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, hit_recovery, jump, att_element, att_active_status, att_active_status_ratio, att_active_status_pow, att_backforce, att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, stuck_rate, att_defenseIgnore, skill_levelup, set_type, url, jewel_type, detail_explain, flavor_text, anti_evil, value, required_skill, need_material, physical_absolute_damage, physical_damage_reduce, physical_absolute_defense, magical_absolute_damage, magical_damage_reduce, magical_absolute_defense, fire_attack, water_attack, dark_attack, light_attack, weapon_break_resistance, armor_break_resistance, all_activestatus_resistance, rigidity, item_aura, magical_critical_hit, set_name, set_item, fullset_basic_explain, fullset_detail_explain, parameter_basic_explain, parameter_detail_explain, part_set_index, skill_data_up, hide_equipment, db_piece_count, set_item_master, it_set_no, ani_variation, ani_variation_expand, ani_variation2, ani_variation_expand2, hide_growtype_avatar, room_list_move_speed_rate, icon_mark_number, extra_icon_idx_list, hp_max_rate, mp_max_rate, all_attack FROM dnf_item_info WHERE it_no = %d AND it_set_no = %d AND db_piece_count = %d",
        it_no, it_set_no, db_piece_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
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
    if (row[73]) strncpy(record->jewel_type, row[73], sizeof(record->jewel_type) - 1);
    if (row[74]) strncpy(record->detail_explain, row[74], sizeof(record->detail_explain) - 1);
    if (row[75]) strncpy(record->flavor_text, row[75], sizeof(record->flavor_text) - 1);
    record->anti_evil = row[76] ? atoi(row[76]) : 0;
    record->value = row[77] ? atoi(row[77]) : 0;
    record->required_skill = row[78] ? atoi(row[78]) : 0;
    if (row[79]) strncpy(record->need_material, row[79], sizeof(record->need_material) - 1);
    record->physical_absolute_damage = row[80] ? atoi(row[80]) : 0;
    record->physical_damage_reduce = row[81] ? atoi(row[81]) : 0;
    record->physical_absolute_defense = row[82] ? atoi(row[82]) : 0;
    record->magical_absolute_damage = row[83] ? atoi(row[83]) : 0;
    record->magical_damage_reduce = row[84] ? atoi(row[84]) : 0;
    record->magical_absolute_defense = row[85] ? atoi(row[85]) : 0;
    record->fire_attack = row[86] ? atoi(row[86]) : 0;
    record->water_attack = row[87] ? atoi(row[87]) : 0;
    record->dark_attack = row[88] ? atoi(row[88]) : 0;
    record->light_attack = row[89] ? atoi(row[89]) : 0;
    record->weapon_break_resistance = row[90] ? atoi(row[90]) : 0;
    record->armor_break_resistance = row[91] ? atoi(row[91]) : 0;
    record->all_activestatus_resistance = row[92] ? atoi(row[92]) : 0;
    record->rigidity = row[93] ? atoi(row[93]) : 0;
    if (row[94]) strncpy(record->item_aura, row[94], sizeof(record->item_aura) - 1);
    record->magical_critical_hit = row[95] ? atof(row[95]) : 0;
    if (row[96]) strncpy(record->set_name, row[96], sizeof(record->set_name) - 1);
    if (row[97]) strncpy(record->set_item, row[97], sizeof(record->set_item) - 1);
    if (row[98]) strncpy(record->fullset_basic_explain, row[98], sizeof(record->fullset_basic_explain) - 1);
    if (row[99]) strncpy(record->fullset_detail_explain, row[99], sizeof(record->fullset_detail_explain) - 1);
    if (row[100]) strncpy(record->parameter_basic_explain, row[100], sizeof(record->parameter_basic_explain) - 1);
    if (row[101]) strncpy(record->parameter_detail_explain, row[101], sizeof(record->parameter_detail_explain) - 1);
    record->part_set_index = row[102] ? atoi(row[102]) : 0;
    if (row[103]) strncpy(record->skill_data_up, row[103], sizeof(record->skill_data_up) - 1);
    if (row[104]) strncpy(record->hide_equipment, row[104], sizeof(record->hide_equipment) - 1);
    record->db_piece_count = row[105] ? atoi(row[105]) : 0;
    record->set_item_master = row[106] ? atoi(row[106]) : 0;
    record->it_set_no = row[107] ? atoi(row[107]) : 0;
    if (row[108]) strncpy(record->ani_variation, row[108], sizeof(record->ani_variation) - 1);
    if (row[109]) strncpy(record->ani_variation_expand, row[109], sizeof(record->ani_variation_expand) - 1);
    if (row[110]) strncpy(record->ani_variation2, row[110], sizeof(record->ani_variation2) - 1);
    if (row[111]) strncpy(record->ani_variation_expand2, row[111], sizeof(record->ani_variation_expand2) - 1);
    if (row[112]) strncpy(record->hide_growtype_avatar, row[112], sizeof(record->hide_growtype_avatar) - 1);
    record->room_list_move_speed_rate = row[113] ? atof(row[113]) : 0;
    record->icon_mark_number = row[114] ? atoi(row[114]) : 0;
    if (row[115]) strncpy(record->extra_icon_idx_list, row[115], sizeof(record->extra_icon_idx_list) - 1);
    record->hp_max_rate = row[116] ? atof(row[116]) : 0;
    record->mp_max_rate = row[117] ? atof(row[117]) : 0;
    record->all_attack = row[118] ? atoi(row[118]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfo_GetAll(DBConnectionManager* manager, DnfItemInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[119];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, it_name, it_eng_name, it_explain, master_type, sub_type, job, class, revert, level, skill, create_ratio, rarity, weight, price, cash, medal, durability, cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, hit_recovery, jump, att_element, att_active_status, att_active_status_ratio, att_active_status_pow, att_backforce, att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, stuck_rate, att_defenseIgnore, skill_levelup, set_type, url, jewel_type, detail_explain, flavor_text, anti_evil, value, required_skill, need_material, physical_absolute_damage, physical_damage_reduce, physical_absolute_defense, magical_absolute_damage, magical_damage_reduce, magical_absolute_defense, fire_attack, water_attack, dark_attack, light_attack, weapon_break_resistance, armor_break_resistance, all_activestatus_resistance, rigidity, item_aura, magical_critical_hit, set_name, set_item, fullset_basic_explain, fullset_detail_explain, parameter_basic_explain, parameter_detail_explain, part_set_index, skill_data_up, hide_equipment, db_piece_count, set_item_master, it_set_no, ani_variation, ani_variation_expand, ani_variation2, ani_variation_expand2, hide_growtype_avatar, room_list_move_speed_rate, icon_mark_number, extra_icon_idx_list, hp_max_rate, mp_max_rate, all_attack FROM dnf_item_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
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
        if (row[73]) strncpy(records[count].jewel_type, row[73], sizeof(records[count].jewel_type) - 1);
        if (row[74]) strncpy(records[count].detail_explain, row[74], sizeof(records[count].detail_explain) - 1);
        if (row[75]) strncpy(records[count].flavor_text, row[75], sizeof(records[count].flavor_text) - 1);
        records[count].anti_evil = row[76] ? atoi(row[76]) : 0;
        records[count].value = row[77] ? atoi(row[77]) : 0;
        records[count].required_skill = row[78] ? atoi(row[78]) : 0;
        if (row[79]) strncpy(records[count].need_material, row[79], sizeof(records[count].need_material) - 1);
        records[count].physical_absolute_damage = row[80] ? atoi(row[80]) : 0;
        records[count].physical_damage_reduce = row[81] ? atoi(row[81]) : 0;
        records[count].physical_absolute_defense = row[82] ? atoi(row[82]) : 0;
        records[count].magical_absolute_damage = row[83] ? atoi(row[83]) : 0;
        records[count].magical_damage_reduce = row[84] ? atoi(row[84]) : 0;
        records[count].magical_absolute_defense = row[85] ? atoi(row[85]) : 0;
        records[count].fire_attack = row[86] ? atoi(row[86]) : 0;
        records[count].water_attack = row[87] ? atoi(row[87]) : 0;
        records[count].dark_attack = row[88] ? atoi(row[88]) : 0;
        records[count].light_attack = row[89] ? atoi(row[89]) : 0;
        records[count].weapon_break_resistance = row[90] ? atoi(row[90]) : 0;
        records[count].armor_break_resistance = row[91] ? atoi(row[91]) : 0;
        records[count].all_activestatus_resistance = row[92] ? atoi(row[92]) : 0;
        records[count].rigidity = row[93] ? atoi(row[93]) : 0;
        if (row[94]) strncpy(records[count].item_aura, row[94], sizeof(records[count].item_aura) - 1);
        records[count].magical_critical_hit = row[95] ? atof(row[95]) : 0;
        if (row[96]) strncpy(records[count].set_name, row[96], sizeof(records[count].set_name) - 1);
        if (row[97]) strncpy(records[count].set_item, row[97], sizeof(records[count].set_item) - 1);
        if (row[98]) strncpy(records[count].fullset_basic_explain, row[98], sizeof(records[count].fullset_basic_explain) - 1);
        if (row[99]) strncpy(records[count].fullset_detail_explain, row[99], sizeof(records[count].fullset_detail_explain) - 1);
        if (row[100]) strncpy(records[count].parameter_basic_explain, row[100], sizeof(records[count].parameter_basic_explain) - 1);
        if (row[101]) strncpy(records[count].parameter_detail_explain, row[101], sizeof(records[count].parameter_detail_explain) - 1);
        records[count].part_set_index = row[102] ? atoi(row[102]) : 0;
        if (row[103]) strncpy(records[count].skill_data_up, row[103], sizeof(records[count].skill_data_up) - 1);
        if (row[104]) strncpy(records[count].hide_equipment, row[104], sizeof(records[count].hide_equipment) - 1);
        records[count].db_piece_count = row[105] ? atoi(row[105]) : 0;
        records[count].set_item_master = row[106] ? atoi(row[106]) : 0;
        records[count].it_set_no = row[107] ? atoi(row[107]) : 0;
        if (row[108]) strncpy(records[count].ani_variation, row[108], sizeof(records[count].ani_variation) - 1);
        if (row[109]) strncpy(records[count].ani_variation_expand, row[109], sizeof(records[count].ani_variation_expand) - 1);
        if (row[110]) strncpy(records[count].ani_variation2, row[110], sizeof(records[count].ani_variation2) - 1);
        if (row[111]) strncpy(records[count].ani_variation_expand2, row[111], sizeof(records[count].ani_variation_expand2) - 1);
        if (row[112]) strncpy(records[count].hide_growtype_avatar, row[112], sizeof(records[count].hide_growtype_avatar) - 1);
        records[count].room_list_move_speed_rate = row[113] ? atof(row[113]) : 0;
        records[count].icon_mark_number = row[114] ? atoi(row[114]) : 0;
        if (row[115]) strncpy(records[count].extra_icon_idx_list, row[115], sizeof(records[count].extra_icon_idx_list) - 1);
        records[count].hp_max_rate = row[116] ? atof(row[116]) : 0;
        records[count].mp_max_rate = row[117] ? atof(row[117]) : 0;
        records[count].all_attack = row[118] ? atoi(row[118]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
