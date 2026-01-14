#!/usr/bin/env python3
# Script to generate charac_info_orm.c

output_file = 'src/database/charac_info_orm.c'

# Write the implementation file
with open(output_file, 'w', encoding='utf-8') as f:
    f.write('''#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql.h>
#include "../../include/database/headers/charac_info_orm.h"
#include "../../include/database/headers/db_connection_manager.h"

int CharacInfo_Create(DBConnectionManager* manager, CharacInfo* info) {
    char query[4096];
    DBQueryResult qresult;
    int ret;
    MYSQL* conn;
    
    if (!manager || !info) return -1;
    
    snprintf(query, sizeof(query),
        "INSERT INTO charac_info (m_id, charac_name, village, sex, job, lev, exp, grow_type, expert_job, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "move_speed, attack_speed, cast_speed, fatigue, max_fatigue, premium_fatigue, guild_id, guild_right, "
        "create_time, last_play_time, delete_time, stat_point_apply_flag, competition_point, mercenary_flag, "
        "competition_rank_point, party_invite_grade, mercenary_reserve_type, mercenary_reserve_state, "
        "mercenary_reserve_time, VIP, competition_season_point, competition_season_id, competition_season_rank_point, "
        "competition_season_reward_flag, grow_quest_clear_flag, competition_normal, competition_reward_flag, "
        "competition_reward_time, arad_adventurer, add_phy_defense, add_mag_defense) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, "
        "%d, %d, %d, %d, %d, %d, %d, "
        "%d, %d, %d, %d, %d, %d, %d, %d, "
        "NOW(), NOW(), '0000-00-00 00:00:00', '%s', %d, %d, "
        "%d, %d, %d, %d, "
        "%d, '%s', %d, %d, %d, "
        "%d, %d, %d, %d, "
        "%d, %d, %d, %d)",
        info->m_id, info->charac_name, info->village, info->sex, info->job, info->lev, info->exp, 
        info->grow_type, info->expert_job,
        info->HP, info->maxHP, info->maxMP, info->phy_attack, info->phy_defense, info->mag_attack, info->mag_defense,
        info->move_speed, info->attack_speed, info->cast_speed, info->fatigue, info->max_fatigue, 
        info->premium_fatigue, info->guild_id, info->guild_right,
        info->stat_point_apply_flag, info->competition_point, info->mercenary_flag,
        info->competition_rank_point, info->party_invite_grade, info->mercenary_reserve_type, 
        info->mercenary_reserve_state,
        info->mercenary_reserve_time, info->VIP, info->competition_season_point, info->competition_season_id, 
        info->competition_season_rank_point,
        info->competition_season_reward_flag, info->grow_quest_clear_flag, info->competition_normal, 
        info->competition_reward_flag,
        info->competition_reward_time, info->arad_adventurer, info->add_phy_defense, info->add_mag_defense);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    if (ret < 0) {
        return -1;
    }

    conn = (MYSQL*)DBConnectionManager_GetConnection(manager, DB_TYPE_CAIN);
    return (int)mysql_insert_id(conn);
}

int CharacInfo_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacInfo* info) {
    char query[2048];
    char* values[51];
    DBQueryResult qresult;
    int ret;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, grow_type, expert_job, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, element_resist, spec_property, "
        "move_speed, attack_speed, cast_speed, fatigue, max_fatigue, premium_fatigue, guild_id, guild_right, "
        "create_time, last_play_time, delete_time, stat_point_apply_flag, competition_point, mercenary_flag, "
        "competition_rank_point, party_invite_grade, mercenary_reserve_type, mercenary_reserve_state, "
        "mercenary_reserve_time, VIP, competition_season_point, competition_season_id, competition_season_rank_point, "
        "competition_season_reward_flag, grow_quest_clear_flag, competition_normal, competition_reward_flag, "
        "competition_reward_time, arad_adventurer, add_phy_defense, add_mag_defense, total_play_time "
        "FROM charac_info WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret < 0 || qresult.row_count == 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    if (DBQueryResult_FetchRow(&qresult, values) > 0) {
        info->charac_no = atoi(values[0]);
        info->m_id = atoi(values[1]);
        strncpy(info->charac_name, values[2], sizeof(info->charac_name) - 1);
        info->charac_name[sizeof(info->charac_name) - 1] = '\0';
        info->village = atoi(values[3]);
        info->sex = atoi(values[4]);
        info->job = atoi(values[5]);
        info->lev = atoi(values[6]);
        info->exp = atoi(values[7]);
        info->grow_type = atoi(values[8]);
        info->expert_job = atoi(values[9]);
        info->HP = atoi(values[10]);
        info->maxHP = atoi(values[11]);
        info->maxMP = atoi(values[12]);
        info->phy_attack = atoi(values[13]);
        info->phy_defense = atoi(values[14]);
        info->mag_attack = atoi(values[15]);
        info->mag_defense = atoi(values[16]);
        if (values[17]) strncpy(info->element_resist, values[17], sizeof(info->element_resist) - 1);
        if (values[18]) strncpy(info->spec_property, values[18], sizeof(info->spec_property) - 1);
        info->move_speed = atoi(values[19]);
        info->attack_speed = atoi(values[20]);
        info->cast_speed = atoi(values[21]);
        info->fatigue = atoi(values[22]);
        info->max_fatigue = atoi(values[23]);
        info->premium_fatigue = atoi(values[24]);
        info->guild_id = atoi(values[25]);
        info->guild_right = atoi(values[26]);
        strncpy(info->create_time, values[27], sizeof(info->create_time) - 1);
        strncpy(info->last_play_time, values[28], sizeof(info->last_play_time) - 1);
        strncpy(info->delete_time, values[29], sizeof(info->delete_time) - 1);
        strncpy(info->stat_point_apply_flag, values[30], sizeof(info->stat_point_apply_flag) - 1);
        info->competition_point = atoi(values[31]);
        info->mercenary_flag = atoi(values[32]);
        info->competition_rank_point = atoi(values[33]);
        info->party_invite_grade = atoi(values[34]);
        info->mercenary_reserve_type = atoi(values[35]);
        info->mercenary_reserve_state = atoi(values[36]);
        info->mercenary_reserve_time = atoi(values[37]);
        strncpy(info->VIP, values[38], sizeof(info->VIP) - 1);
        info->competition_season_point = atoi(values[39]);
        info->competition_season_id = atoi(values[40]);
        info->competition_season_rank_point = atoi(values[41]);
        info->competition_season_reward_flag = atoi(values[42]);
        info->grow_quest_clear_flag = atoi(values[43]);
        info->competition_normal = atoi(values[44]);
        info->competition_reward_flag = atoi(values[45]);
        info->competition_reward_time = atoi(values[46]);
        info->arad_adventurer = atoi(values[47]);
        info->add_phy_defense = atoi(values[48]);
        info->add_mag_defense = atoi(values[49]);
    }

    DBQueryResult_Free(&qresult);
    return 0;
}
''')

print(f"Generated: {output_file}")
