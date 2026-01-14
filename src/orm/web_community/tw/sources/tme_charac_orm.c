#include "tme_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TmeCharac_Add(DBConnectionManager* manager, const TmeCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tme_charac (m_id, charac_no, charac_name, village, job, lev, exp, grow_type, HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, inven_weight, hp_regen, mp_regen, move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, guild_id, guild_right, member_flag) "
        "VALUES (%d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', %d, '%s', %d, %d, %d, %d)",
        record->m_id, record->charac_no, record->charac_name, record->village, record->job, record->lev, record->exp, record->grow_type, record->HP, record->maxHP, record->maxMP, record->phy_attack, record->phy_defense, record->mag_attack, record->mag_defense, record->inven_weight, record->hp_regen, record->mp_regen, record->move_speed, record->attack_speed, record->cast_speed, record->hit_recovery, record->jump, record->charac_weight, record->fatigue, record->max_fatigue, record->premium_fatigue, record->max_premium_fatigue, record->create_time, record->last_play_time, record->dungeon_clear_point, record->delete_time, record->delete_flag, record->guild_id, record->guild_right, record->member_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_GetAll(DBConnectionManager* manager, TmeCharac* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[36];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, inven_weight, hp_regen, mp_regen, move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, guild_id, guild_right, member_flag FROM tme_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TmeCharac));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].charac_name, row[2], sizeof(records[count].charac_name) - 1);
        records[count].village = row[3] ? atoi(row[3]) : 0;
        records[count].job = row[4] ? atoi(row[4]) : 0;
        records[count].lev = row[5] ? atoi(row[5]) : 0;
        records[count].exp = row[6] ? atoi(row[6]) : 0;
        records[count].grow_type = row[7] ? atoi(row[7]) : 0;
        records[count].HP = row[8] ? atoi(row[8]) : 0;
        records[count].maxHP = row[9] ? atoi(row[9]) : 0;
        records[count].maxMP = row[10] ? atoi(row[10]) : 0;
        records[count].phy_attack = row[11] ? atoi(row[11]) : 0;
        records[count].phy_defense = row[12] ? atoi(row[12]) : 0;
        records[count].mag_attack = row[13] ? atoi(row[13]) : 0;
        records[count].mag_defense = row[14] ? atoi(row[14]) : 0;
        records[count].inven_weight = row[15] ? atoi(row[15]) : 0;
        records[count].hp_regen = row[16] ? atoi(row[16]) : 0;
        records[count].mp_regen = row[17] ? atoi(row[17]) : 0;
        records[count].move_speed = row[18] ? atoi(row[18]) : 0;
        records[count].attack_speed = row[19] ? atoi(row[19]) : 0;
        records[count].cast_speed = row[20] ? atoi(row[20]) : 0;
        records[count].hit_recovery = row[21] ? atoi(row[21]) : 0;
        records[count].jump = row[22] ? atoi(row[22]) : 0;
        records[count].charac_weight = row[23] ? atoi(row[23]) : 0;
        records[count].fatigue = row[24] ? atoi(row[24]) : 0;
        records[count].max_fatigue = row[25] ? atoi(row[25]) : 0;
        records[count].premium_fatigue = row[26] ? atoi(row[26]) : 0;
        records[count].max_premium_fatigue = row[27] ? atoi(row[27]) : 0;
        if (row[28]) strncpy(records[count].create_time, row[28], sizeof(records[count].create_time) - 1);
        if (row[29]) strncpy(records[count].last_play_time, row[29], sizeof(records[count].last_play_time) - 1);
        records[count].dungeon_clear_point = row[30] ? atoi(row[30]) : 0;
        if (row[31]) strncpy(records[count].delete_time, row[31], sizeof(records[count].delete_time) - 1);
        records[count].delete_flag = row[32] ? atoi(row[32]) : 0;
        records[count].guild_id = row[33] ? atoi(row[33]) : 0;
        records[count].guild_right = row[34] ? atoi(row[34]) : 0;
        records[count].member_flag = row[35] ? atoi(row[35]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
