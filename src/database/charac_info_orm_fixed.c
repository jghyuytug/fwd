/**
 * @file charac_info_orm.c
 * @brief Character Information ORM Implementation
 * Database: taiwan_cain (DB_TYPE_CAIN = 2)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "../../include/database/headers/charac_info_orm.h"
#include "../../include/database/headers/db_connection_manager.h"

int CharacInfo_Create(DBConnectionManager* manager, CharacInfo* info) {
    char query[4096];
    char* values[1];
    DBQueryResult qresult;
    int ret, charac_no;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_info (m_id, charac_name, village, sex, job, lev, exp, grow_type, expert_job, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "move_speed, attack_speed, cast_speed, fatigue, max_fatigue, premium_fatigue, guild_id, guild_right) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        info->m_id, info->charac_name, info->village, info->sex, info->job, info->lev, info->exp,
        info->grow_type, info->expert_job, info->HP, info->maxHP, info->maxMP, info->phy_attack,
        info->phy_defense, info->mag_attack, info->mag_defense, info->move_speed, info->attack_speed,
        info->cast_speed, info->fatigue, info->max_fatigue, info->premium_fatigue, info->guild_id, info->guild_right);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    if (ret < 0) return -1;

    /* Get the last insert ID */
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0 || DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    charac_no = atoi(values[0]);
    DBQueryResult_Free(&qresult);
    return charac_no;
}


int CharacInfo_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacInfo* info) {
    char query[2048];
    char* values[51];
    DBQueryResult qresult;
    int ret;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, grow_type, expert_job, "
        "skill_tree_index, HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "element_resist, spec_property, move_speed, attack_speed, cast_speed, hit_recovery, jump, "
        "inven_weight, charac_weight, fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, delete_time, delete_flag, guild_id, guild_right, guild_secede, "
        "member_flag, dungeon_clear_point, link_charac_no, event_charac_level, VIP, start_time, "
        "finish_time, competition_area, competition_period, mercenary_start_time, mercenary_finish_time, "
        "mercenary_area, mercenary_period "
        "FROM charac_info WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    if (DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return 0;  /* Record not found */
    }

    memset(info, 0, sizeof(CharacInfo));
    /* Map 51 fields from actual database */
    info->charac_no = atoi(values[0]);
    info->m_id = atoi(values[1]);
    strncpy(info->charac_name, values[2], sizeof(info->charac_name)-1);
    info->village = (signed char)atoi(values[3]);
    info->sex = (signed char)atoi(values[4]);
    info->job = (signed char)atoi(values[5]);
    info->lev = (unsigned char)atoi(values[6]);
    info->exp = atoi(values[7]);
    info->grow_type = (unsigned char)atoi(values[8]);
    info->expert_job = (signed char)atoi(values[9]);
    /* values[10] = skill_tree_index - not in struct */
    info->HP = atoi(values[11]);
    info->maxHP = atoi(values[12]);
    info->maxMP = atoi(values[13]);
    info->phy_attack = atoi(values[14]);
    info->phy_defense = atoi(values[15]);
    info->mag_attack = atoi(values[16]);
    info->mag_defense = atoi(values[17]);
    if (values[18]) strncpy(info->element_resist, values[18], sizeof(info->element_resist)-1);
    if (values[19]) strncpy(info->spec_property, values[19], sizeof(info->spec_property)-1);
    info->move_speed = atoi(values[20]);
    info->attack_speed = atoi(values[21]);
    info->cast_speed = atoi(values[22]);
    /* values[23] = hit_recovery, values[24] = jump - not in struct */
    /* values[25] = inven_weight, values[26] = charac_weight - not in struct */
    info->fatigue = atoi(values[27]);
    info->max_fatigue = atoi(values[28]);
    info->premium_fatigue = atoi(values[29]);
    /* values[30] = max_premium_fatigue - not in struct */
    strncpy(info->create_time, values[31], sizeof(info->create_time)-1);
    strncpy(info->last_play_time, values[32], sizeof(info->last_play_time)-1);
    if (values[33]) strncpy(info->delete_time, values[33], sizeof(info->delete_time)-1);
    /* values[34] = delete_flag - not in struct */
    info->guild_id = atoi(values[35]);
    info->guild_right = (signed char)atoi(values[36]);
    /* values[37] = guild_secede, values[38] = member_flag - not in struct */
    /* values[39] = dungeon_clear_point, values[40] = link_charac_no, values[41] = event_charac_level */
    if (values[42]) strncpy(info->VIP, values[42], sizeof(info->VIP)-1);
    /* values[43] = start_time, values[44] = finish_time */
    /* values[45] = competition_area, values[46] = competition_period */
    /* values[47] = mercenary_start_time, values[48] = mercenary_finish_time */
    /* values[49] = mercenary_area, values[50] = mercenary_period */

    DBQueryResult_Free(&qresult);
    return 0;
}

int CharacInfo_GetByName(DBConnectionManager* manager, const char* charac_name, CharacInfo* info) {
    char query[2048];
    char* values[51];
    DBQueryResult qresult;
    int ret;

    if (!manager || !charac_name || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, grow_type, expert_job, "
        "skill_tree_index, HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "element_resist, spec_property, move_speed, attack_speed, cast_speed, hit_recovery, jump, "
        "inven_weight, charac_weight, fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, delete_time, delete_flag, guild_id, guild_right, guild_secede, "
        "member_flag, dungeon_clear_point, link_charac_no, event_charac_level, VIP, start_time, "
        "finish_time, competition_area, competition_period, mercenary_start_time, mercenary_finish_time, "
        "mercenary_area, mercenary_period "
        "FROM charac_info WHERE charac_name='%s'", charac_name);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0 || DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    memset(info, 0, sizeof(CharacInfo));
    /* Map 51 fields from actual database */
    info->charac_no = atoi(values[0]);
    info->m_id = atoi(values[1]);
    strncpy(info->charac_name, values[2], sizeof(info->charac_name)-1);
    info->village = (signed char)atoi(values[3]);
    info->sex = (signed char)atoi(values[4]);
    info->job = (signed char)atoi(values[5]);
    info->lev = (unsigned char)atoi(values[6]);
    info->exp = atoi(values[7]);
    info->grow_type = (unsigned char)atoi(values[8]);
    info->expert_job = (signed char)atoi(values[9]);
    /* values[10] = skill_tree_index - not in struct */
    info->HP = atoi(values[11]);
    info->maxHP = atoi(values[12]);
    info->maxMP = atoi(values[13]);
    info->phy_attack = atoi(values[14]);
    info->phy_defense = atoi(values[15]);
    info->mag_attack = atoi(values[16]);
    info->mag_defense = atoi(values[17]);
    if (values[18]) strncpy(info->element_resist, values[18], sizeof(info->element_resist)-1);
    if (values[19]) strncpy(info->spec_property, values[19], sizeof(info->spec_property)-1);
    info->move_speed = atoi(values[20]);
    info->attack_speed = atoi(values[21]);
    info->cast_speed = atoi(values[22]);
    /* values[23] = hit_recovery, values[24] = jump - not in struct */
    /* values[25] = inven_weight, values[26] = charac_weight - not in struct */
    info->fatigue = atoi(values[27]);
    info->max_fatigue = atoi(values[28]);
    info->premium_fatigue = atoi(values[29]);
    /* values[30] = max_premium_fatigue - not in struct */
    strncpy(info->create_time, values[31], sizeof(info->create_time)-1);
    strncpy(info->last_play_time, values[32], sizeof(info->last_play_time)-1);
    if (values[33]) strncpy(info->delete_time, values[33], sizeof(info->delete_time)-1);
    /* values[34] = delete_flag - not in struct */
    info->guild_id = atoi(values[35]);
    info->guild_right = (signed char)atoi(values[36]);
    /* values[37] = guild_secede, values[38] = member_flag - not in struct */
    /* values[39] = dungeon_clear_point, values[40] = link_charac_no, values[41] = event_charac_level */
    if (values[42]) strncpy(info->VIP, values[42], sizeof(info->VIP)-1);
    /* values[43] = start_time, values[44] = finish_time */
    /* values[45] = competition_area, values[46] = competition_period */
    /* values[47] = mercenary_start_time, values[48] = mercenary_finish_time */
    /* values[49] = mercenary_area, values[50] = mercenary_period */

    DBQueryResult_Free(&qresult);
    return 0;
}

int CharacInfo_Update(DBConnectionManager* manager, CharacInfo* info) {
    char query[4096];
    DBQueryResult qresult;
    int ret;

    if (!manager || !info) return -1;

    /* Only update fields that exist in both database and CharacInfo struct */
    snprintf(query, sizeof(query),
        "UPDATE charac_info SET "
        "m_id=%d, charac_name='%s', village=%d, sex=%d, job=%d, lev=%d, exp=%d, "
        "grow_type=%d, expert_job=%d, HP=%d, maxHP=%d, maxMP=%d, "
        "phy_attack=%d, phy_defense=%d, mag_attack=%d, mag_defense=%d, "
        "element_resist='%s', spec_property='%s', "
        "move_speed=%d, attack_speed=%d, cast_speed=%d, "
        "fatigue=%d, max_fatigue=%d, premium_fatigue=%d, "
        "guild_id=%d, guild_right=%d, VIP='%s' "
        "WHERE charac_no=%d",
        info->m_id, info->charac_name, info->village, info->sex, info->job, info->lev, info->exp,
        info->grow_type, info->expert_job, info->HP, info->maxHP, info->maxMP,
        info->phy_attack, info->phy_defense, info->mag_attack, info->mag_defense,
        info->element_resist, info->spec_property,
        info->move_speed, info->attack_speed, info->cast_speed,
        info->fatigue, info->max_fatigue, info->premium_fatigue,
        info->guild_id, info->guild_right, info->VIP, info->charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return ret;
}

int CharacInfo_Delete(DBConnectionManager* manager, int charac_no) {
    char query[512];
    DBQueryResult qresult;
    int ret;
    
    if (!manager) return -1;
    
    snprintf(query, sizeof(query), "DELETE FROM charac_info WHERE charac_no=%d", charac_no);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    
    return ret;
}

int CharacInfo_Exists(DBConnectionManager* manager, int charac_no) {
    char query[512];
    char* values[1];
    DBQueryResult qresult;
    int ret;
    
    if (!manager) return 0;
    
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_info WHERE charac_no=%d", charac_no);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    
    if (ret <= 0 || DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return 0;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&qresult);
    
    return (count > 0) ? 1 : 0;
}

int CharacInfo_UpdateLevelExp(DBConnectionManager* manager, int charac_no, unsigned char lev, int exp) {
    char query[512];
    DBQueryResult qresult;
    int ret;
    
    if (!manager) return -1;
    
    snprintf(query, sizeof(query),
        "UPDATE charac_info SET lev=%d, exp=%d WHERE charac_no=%d",
        (int)lev, exp, charac_no);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    
    return ret;
}

int CharacInfo_UpdateCombatStats(DBConnectionManager* manager, int charac_no, int HP, int maxHP, int maxMP) {
    char query[512];
    DBQueryResult qresult;
    int ret;
    
    if (!manager) return -1;
    
    snprintf(query, sizeof(query),
        "UPDATE charac_info SET HP=%d, maxHP=%d, maxMP=%d WHERE charac_no=%d",
        HP, maxHP, maxMP, charac_no);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    
    return ret;
}

int CharacInfo_UpdateGuild(DBConnectionManager* manager, int charac_no, int guild_id, signed char guild_right) {
    char query[512];
    DBQueryResult qresult;
    int ret;
    
    if (!manager) return -1;
    
    snprintf(query, sizeof(query),
        "UPDATE charac_info SET guild_id=%d, guild_right=%d WHERE charac_no=%d",
        guild_id, (int)guild_right, charac_no);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    
    return ret;
}

int CharacInfo_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no) {
    char query[512];
    DBQueryResult qresult;
    int ret;
    
    if (!manager) return -1;
    
    snprintf(query, sizeof(query),
        "UPDATE charac_info SET last_play_time=NOW() WHERE charac_no=%d", charac_no);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    
    return ret;
}

int CharacInfo_GetByMemberId(DBConnectionManager* manager, int m_id, CharacInfo* results, int max_count) {
    char query[2048];
    char* values[51];
    DBQueryResult qresult;
    int ret, i;

    if (!manager || !results || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, grow_type, expert_job, "
        "skill_tree_index, HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "element_resist, spec_property, move_speed, attack_speed, cast_speed, hit_recovery, jump, "
        "inven_weight, charac_weight, fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, delete_time, delete_flag, guild_id, guild_right, guild_secede, "
        "member_flag, dungeon_clear_point, link_charac_no, event_charac_level, VIP, start_time, "
        "finish_time, competition_area, competition_period, mercenary_start_time, mercenary_finish_time, "
        "mercenary_area, mercenary_period "
        "FROM charac_info WHERE m_id=%d AND delete_time IS NULL LIMIT %d", m_id, max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0) {
        DBQueryResult_Free(&qresult);
        return 0;
    }

    i = 0;
    while (i < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        memset(&results[i], 0, sizeof(CharacInfo));
        /* Map 51 fields from actual database */
        results[i].charac_no = atoi(values[0]);
        results[i].m_id = atoi(values[1]);
        strncpy(results[i].charac_name, values[2], sizeof(results[i].charac_name)-1);
        results[i].village = (signed char)atoi(values[3]);
        results[i].sex = (signed char)atoi(values[4]);
        results[i].job = (signed char)atoi(values[5]);
        results[i].lev = (unsigned char)atoi(values[6]);
        results[i].exp = atoi(values[7]);
        results[i].grow_type = (unsigned char)atoi(values[8]);
        results[i].expert_job = (signed char)atoi(values[9]);
        /* values[10] = skill_tree_index - not in struct */
        results[i].HP = atoi(values[11]);
        results[i].maxHP = atoi(values[12]);
        results[i].maxMP = atoi(values[13]);
        results[i].phy_attack = atoi(values[14]);
        results[i].phy_defense = atoi(values[15]);
        results[i].mag_attack = atoi(values[16]);
        results[i].mag_defense = atoi(values[17]);
        if (values[18]) strncpy(results[i].element_resist, values[18], sizeof(results[i].element_resist)-1);
        if (values[19]) strncpy(results[i].spec_property, values[19], sizeof(results[i].spec_property)-1);
        results[i].move_speed = atoi(values[20]);
        results[i].attack_speed = atoi(values[21]);
        results[i].cast_speed = atoi(values[22]);
        /* values[23] = hit_recovery, values[24] = jump - not in struct */
        /* values[25] = inven_weight, values[26] = charac_weight - not in struct */
        results[i].fatigue = atoi(values[27]);
        results[i].max_fatigue = atoi(values[28]);
        results[i].premium_fatigue = atoi(values[29]);
        /* values[30] = max_premium_fatigue - not in struct */
        strncpy(results[i].create_time, values[31], sizeof(results[i].create_time)-1);
        strncpy(results[i].last_play_time, values[32], sizeof(results[i].last_play_time)-1);
        if (values[33]) strncpy(results[i].delete_time, values[33], sizeof(results[i].delete_time)-1);
        /* values[34] = delete_flag - not in struct */
        results[i].guild_id = atoi(values[35]);
        results[i].guild_right = (signed char)atoi(values[36]);
        /* values[37] = guild_secede, values[38] = member_flag - not in struct */
        /* values[39] = dungeon_clear_point, values[40] = link_charac_no, values[41] = event_charac_level */
        if (values[42]) strncpy(results[i].VIP, values[42], sizeof(results[i].VIP)-1);
        /* values[43] = start_time, values[44] = finish_time */
        /* values[45] = competition_area, values[46] = competition_period */
        /* values[47] = mercenary_start_time, values[48] = mercenary_finish_time */
        /* values[49] = mercenary_area, values[50] = mercenary_period */
        i++;
    }

    DBQueryResult_Free(&qresult);
    return i;
}

int CharacInfo_GetCountByMemberId(DBConnectionManager* manager, int m_id) {
    char query[512];
    char* values[1];
    DBQueryResult qresult;
    int ret, count;
    
    if (!manager) return -1;
    
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_info WHERE m_id=%d AND delete_time IS NULL", m_id);
    
    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    
    if (ret <= 0 || DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    count = atoi(values[0]);
    DBQueryResult_Free(&qresult);

    return count;
}
