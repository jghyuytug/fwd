/**
 * @file charac_stat_orm.c
 * @brief Character Statistics ORM Implementation
 *
 * Database: taiwan_cain (DB_TYPE_CAIN = 2)
 */

#include "charac_stat_orm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int CharacStat_Create(DBConnectionManager* manager, CharacStat* stat) {
    char query[4096];
    DBQueryResult qresult;
    int ret;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
             "INSERT INTO charac_stat ("
             "charac_no, village, exp, HP, fatigue, used_fatigue, premium_fatigue, "
             "dungeon_clear_point, last_play_time, forbidden_to_play, forbidden_due_to, tutorial_flag, "
             "trade_gold_total, trade_gold_total_billion, trade_gold_daily, "
             "dungeon_map_pass_cnt, dungeon_map_help_pass_cnt, help_abuse_point, "
             "chaos_point, chaos_exp, chaos_mode_count, chaos_kill_count, chaos_die_count, "
             "chaos_die_time, chaos_kill_time, assault_count, luck_point, dungeon_play_count, "
             "help_abuse_ratio, help_abuse_exp, expert_job_exp, fatigue_battery_charging, "
             "escalade_tutorial_flag, power_war_point, power_war_assault_count, "
             "power_war_assault_victory_count, fatigue_grownup_buff, village_prev, "
             "last_play_time_powerwar, emotion, add_slot_flag, member_dungeon_flag, open_flag, "
             "member_bonus_fatigue, birthday_effect_time, visible_flags, add_equipslot_flag, "
             "channel_equipslot_switch, expand_equipslot_switch, growth_power_reward, "
             "chaos_respon_time, last_play_dungeon_index, total_play_time"
             ") VALUES ("
             "%d, %d, %d, %u, %d, %d, %d, "
             "%d, '%s', '%s', '%s', %d, "
             "%u, %u, %u, "
             "%u, %u, %u, "
             "%u, %u, %u, %u, %u, "
             "'%s', '%s', %u, %d, %u, "
             "%d, %d, %d, %d, "
             "'%s', %u, %u, "
             "%u, %u, %d, "
             "'%s', %u, %u, %u, %d, "
             "%u, '%s', %u, %d, "
             "%d, %d, %d, "
             "'%s', %u, %u"
             ")",
             stat->charac_no, stat->village, stat->exp, stat->HP, stat->fatigue, stat->used_fatigue, stat->premium_fatigue,
             stat->dungeon_clear_point, stat->last_play_time, stat->forbidden_to_play, stat->forbidden_due_to, stat->tutorial_flag,
             stat->trade_gold_total, stat->trade_gold_total_billion, stat->trade_gold_daily,
             stat->dungeon_map_pass_cnt, stat->dungeon_map_help_pass_cnt, stat->help_abuse_point,
             stat->chaos_point, stat->chaos_exp, stat->chaos_mode_count, stat->chaos_kill_count, stat->chaos_die_count,
             stat->chaos_die_time, stat->chaos_kill_time, stat->assault_count, stat->luck_point, stat->dungeon_play_count,
             stat->help_abuse_ratio, stat->help_abuse_exp, stat->expert_job_exp, stat->fatigue_battery_charging,
             stat->escalade_tutorial_flag, stat->power_war_point, stat->power_war_assault_count,
             stat->power_war_assault_victory_count, stat->fatigue_grownup_buff, stat->village_prev,
             stat->last_play_time_powerwar, stat->emotion, stat->add_slot_flag, stat->member_dungeon_flag, stat->open_flag,
             stat->member_bonus_fatigue, stat->birthday_effect_time, stat->visible_flags, stat->add_equipslot_flag,
             stat->channel_equipslot_switch, stat->expand_equipslot_switch, stat->growth_power_reward,
             stat->chaos_respon_time, stat->last_play_dungeon_index, stat->total_play_time);

    printf("[DEBUG] SQL Query length: %zu\n", strlen(query));
    printf("[DEBUG] Complete SQL Query:\n%s\n", query);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    printf("[DEBUG] ExecuteQuery returned: %d\n", ret);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacStat* stat) {
    char query[1024];
    char* values[53];
    DBQueryResult qresult;
    int ret;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, village, exp, HP, fatigue, used_fatigue, premium_fatigue, "
             "dungeon_clear_point, last_play_time, forbidden_to_play, forbidden_due_to, tutorial_flag, "
             "trade_gold_total, trade_gold_total_billion, trade_gold_daily, "
             "dungeon_map_pass_cnt, dungeon_map_help_pass_cnt, help_abuse_point, "
             "chaos_point, chaos_exp, chaos_mode_count, chaos_kill_count, chaos_die_count, "
             "chaos_die_time, chaos_kill_time, assault_count, luck_point, dungeon_play_count, "
             "help_abuse_ratio, help_abuse_exp, expert_job_exp, fatigue_battery_charging, "
             "escalade_tutorial_flag, power_war_point, power_war_assault_count, "
             "power_war_assault_victory_count, fatigue_grownup_buff, village_prev, "
             "last_play_time_powerwar, emotion, add_slot_flag, member_dungeon_flag, open_flag, "
             "member_bonus_fatigue, birthday_effect_time, visible_flags, add_equipslot_flag, "
             "channel_equipslot_switch, expand_equipslot_switch, growth_power_reward, "
             "chaos_respon_time, last_play_dungeon_index, total_play_time "
             "FROM charac_stat WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    if (DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    /* Extract all 53 fields */
    stat->charac_no = atoi(values[0]);
    stat->village = atoi(values[1]);
    stat->exp = atoi(values[2]);
    stat->HP = atoi(values[3]);
    stat->fatigue = atoi(values[4]);
    stat->used_fatigue = atoi(values[5]);
    stat->premium_fatigue = atoi(values[6]);
    stat->dungeon_clear_point = atoi(values[7]);
    strncpy(stat->last_play_time, values[8] ? values[8] : "", sizeof(stat->last_play_time) - 1);
    strncpy(stat->forbidden_to_play, values[9] ? values[9] : "", sizeof(stat->forbidden_to_play) - 1);
    strncpy(stat->forbidden_due_to, values[10] ? values[10] : "", sizeof(stat->forbidden_due_to) - 1);
    stat->tutorial_flag = atoi(values[11]);
    stat->trade_gold_total = atoi(values[12]);
    stat->trade_gold_total_billion = atoi(values[13]);
    stat->trade_gold_daily = atoi(values[14]);
    stat->dungeon_map_pass_cnt = atoi(values[15]);
    stat->dungeon_map_help_pass_cnt = atoi(values[16]);
    stat->help_abuse_point = atoi(values[17]);
    stat->chaos_point = atoi(values[18]);
    stat->chaos_exp = atoi(values[19]);
    stat->chaos_mode_count = atoi(values[20]);
    stat->chaos_kill_count = atoi(values[21]);
    stat->chaos_die_count = atoi(values[22]);
    strncpy(stat->chaos_die_time, values[23] ? values[23] : "", sizeof(stat->chaos_die_time) - 1);
    strncpy(stat->chaos_kill_time, values[24] ? values[24] : "", sizeof(stat->chaos_kill_time) - 1);
    stat->assault_count = atoi(values[25]);
    stat->luck_point = atoi(values[26]);
    stat->dungeon_play_count = atoi(values[27]);
    stat->help_abuse_ratio = atoi(values[28]);
    stat->help_abuse_exp = atoi(values[29]);
    stat->expert_job_exp = atoi(values[30]);
    stat->fatigue_battery_charging = atoi(values[31]);
    strncpy(stat->escalade_tutorial_flag, values[32] ? values[32] : "", sizeof(stat->escalade_tutorial_flag) - 1);
    stat->power_war_point = atoi(values[33]);
    stat->power_war_assault_count = atoi(values[34]);
    stat->power_war_assault_victory_count = atoi(values[35]);
    stat->fatigue_grownup_buff = atoi(values[36]);
    stat->village_prev = atoi(values[37]);
    strncpy(stat->last_play_time_powerwar, values[38] ? values[38] : "", sizeof(stat->last_play_time_powerwar) - 1);
    stat->emotion = atoi(values[39]);
    stat->add_slot_flag = atoi(values[40]);
    stat->member_dungeon_flag = atoi(values[41]);
    stat->open_flag = atoi(values[42]);
    stat->member_bonus_fatigue = atoi(values[43]);
    strncpy(stat->birthday_effect_time, values[44] ? values[44] : "", sizeof(stat->birthday_effect_time) - 1);
    stat->visible_flags = atoi(values[45]);
    stat->add_equipslot_flag = atoi(values[46]);
    stat->channel_equipslot_switch = atoi(values[47]);
    stat->expand_equipslot_switch = atoi(values[48]);
    stat->growth_power_reward = atoi(values[49]);
    strncpy(stat->chaos_respon_time, values[50] ? values[50] : "", sizeof(stat->chaos_respon_time) - 1);
    stat->last_play_dungeon_index = atoi(values[51]);
    stat->total_play_time = atoi(values[52]);

    DBQueryResult_Free(&qresult);
    return 0;
}

int CharacStat_Update(DBConnectionManager* manager, CharacStat* stat) {
    char query[4096];
    DBQueryResult qresult;
    int ret;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET "
             "village=%d, exp=%d, HP=%u, fatigue=%d, used_fatigue=%d, premium_fatigue=%d, "
             "dungeon_clear_point=%d, last_play_time='%s', forbidden_to_play='%s', forbidden_due_to='%s', tutorial_flag=%d, "
             "trade_gold_total=%u, trade_gold_total_billion=%u, trade_gold_daily=%u, "
             "dungeon_map_pass_cnt=%u, dungeon_map_help_pass_cnt=%u, help_abuse_point=%u, "
             "chaos_point=%u, chaos_exp=%u, chaos_mode_count=%u, chaos_kill_count=%u, chaos_die_count=%u, "
             "chaos_die_time='%s', chaos_kill_time='%s', assault_count=%u, luck_point=%d, dungeon_play_count=%u, "
             "help_abuse_ratio=%d, help_abuse_exp=%d, expert_job_exp=%d, fatigue_battery_charging=%d, "
             "escalade_tutorial_flag='%s', power_war_point=%u, power_war_assault_count=%u, "
             "power_war_assault_victory_count=%u, fatigue_grownup_buff=%u, village_prev=%d, "
             "last_play_time_powerwar='%s', emotion=%u, add_slot_flag=%u, member_dungeon_flag=%u, open_flag=%d, "
             "member_bonus_fatigue=%u, birthday_effect_time='%s', visible_flags=%u, add_equipslot_flag=%d, "
             "channel_equipslot_switch=%d, expand_equipslot_switch=%d, growth_power_reward=%d, "
             "chaos_respon_time='%s', last_play_dungeon_index=%u, total_play_time=%u "
             "WHERE charac_no=%d",
             stat->village, stat->exp, stat->HP, stat->fatigue, stat->used_fatigue, stat->premium_fatigue,
             stat->dungeon_clear_point, stat->last_play_time, stat->forbidden_to_play, stat->forbidden_due_to, stat->tutorial_flag,
             stat->trade_gold_total, stat->trade_gold_total_billion, stat->trade_gold_daily,
             stat->dungeon_map_pass_cnt, stat->dungeon_map_help_pass_cnt, stat->help_abuse_point,
             stat->chaos_point, stat->chaos_exp, stat->chaos_mode_count, stat->chaos_kill_count, stat->chaos_die_count,
             stat->chaos_die_time, stat->chaos_kill_time, stat->assault_count, stat->luck_point, stat->dungeon_play_count,
             stat->help_abuse_ratio, stat->help_abuse_exp, stat->expert_job_exp, stat->fatigue_battery_charging,
             stat->escalade_tutorial_flag, stat->power_war_point, stat->power_war_assault_count,
             stat->power_war_assault_victory_count, stat->fatigue_grownup_buff, stat->village_prev,
             stat->last_play_time_powerwar, stat->emotion, stat->add_slot_flag, stat->member_dungeon_flag, stat->open_flag,
             stat->member_bonus_fatigue, stat->birthday_effect_time, stat->visible_flags, stat->add_equipslot_flag,
             stat->channel_equipslot_switch, stat->expand_equipslot_switch, stat->growth_power_reward,
             stat->chaos_respon_time, stat->last_play_dungeon_index, stat->total_play_time,
             stat->charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_Delete(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM charac_stat WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_Exists(DBConnectionManager* manager, int charac_no) {
    char query[256];
    char* values[1];
    DBQueryResult qresult;
    int ret;
    int exists = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM charac_stat WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret > 0 && DBQueryResult_FetchRow(&qresult, values) > 0) {
        exists = atoi(values[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&qresult);
    return exists;
}

int CharacStat_UpdateExp(DBConnectionManager* manager, int charac_no, int exp) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET exp=%d WHERE charac_no=%d", exp, charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_UpdateFatigue(DBConnectionManager* manager, int charac_no,
                              short fatigue, short used_fatigue, short premium_fatigue) {
    char query[512];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET fatigue=%d, used_fatigue=%d, premium_fatigue=%d WHERE charac_no=%d",
             fatigue, used_fatigue, premium_fatigue, charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET last_play_time=NOW() WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_IncrementDungeonPlayCount(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET dungeon_play_count=dungeon_play_count+1 WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_UpdateChaosStats(DBConnectionManager* manager, int charac_no,
                                 unsigned int chaos_point, unsigned int chaos_exp,
                                 unsigned int chaos_kill_count, unsigned int chaos_die_count) {
    char query[512];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET chaos_point=%u, chaos_exp=%u, chaos_kill_count=%u, chaos_die_count=%u "
             "WHERE charac_no=%d",
             chaos_point, chaos_exp, chaos_kill_count, chaos_die_count, charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacStat_GetByExpRange(DBConnectionManager* manager, int min_exp, int max_exp,
                              CharacStat* results, int max_count) {
    char query[2048];
    char* values[53];
    DBQueryResult qresult;
    int ret;
    int i, count;

    if (!manager || !results) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, village, exp, HP, fatigue, used_fatigue, premium_fatigue, "
             "dungeon_clear_point, last_play_time, forbidden_to_play, forbidden_due_to, tutorial_flag, "
             "trade_gold_total, trade_gold_total_billion, trade_gold_daily, "
             "dungeon_map_pass_cnt, dungeon_map_help_pass_cnt, help_abuse_point, "
             "chaos_point, chaos_exp, chaos_mode_count, chaos_kill_count, chaos_die_count, "
             "chaos_die_time, chaos_kill_time, assault_count, luck_point, dungeon_play_count, "
             "help_abuse_ratio, help_abuse_exp, expert_job_exp, fatigue_battery_charging, "
             "escalade_tutorial_flag, power_war_point, power_war_assault_count, "
             "power_war_assault_victory_count, fatigue_grownup_buff, village_prev, "
             "last_play_time_powerwar, emotion, add_slot_flag, member_dungeon_flag, open_flag, "
             "member_bonus_fatigue, birthday_effect_time, visible_flags, add_equipslot_flag, "
             "channel_equipslot_switch, expand_equipslot_switch, growth_power_reward, "
             "chaos_respon_time, last_play_dungeon_index, total_play_time "
             "FROM charac_stat WHERE exp BETWEEN %d AND %d LIMIT %d",
             min_exp, max_exp, max_count);

    printf("[DEBUG GetByExpRange] SQL Query:\n%s\n", query);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    printf("[DEBUG GetByExpRange] ExecuteQuery returned: %d, row_count=%d\n", ret, qresult.row_count);

    if (ret <= 0) {
        DBQueryResult_Free(&qresult);
        return 0;
    }

    count = 0;
    for (i = 0; i < max_count; i++) {
        if (DBQueryResult_FetchRow(&qresult, values) <= 0) {
            break;
        }

        /* Extract all fields (same as GetByCharacNo) */
        results[i].charac_no = atoi(values[0]);
        results[i].village = atoi(values[1]);
        results[i].exp = atoi(values[2]);
        results[i].HP = atoi(values[3]);
        results[i].fatigue = atoi(values[4]);
        results[i].used_fatigue = atoi(values[5]);
        results[i].premium_fatigue = atoi(values[6]);
        results[i].dungeon_clear_point = atoi(values[7]);
        strncpy(results[i].last_play_time, values[8] ? values[8] : "", sizeof(results[i].last_play_time) - 1);
        strncpy(results[i].forbidden_to_play, values[9] ? values[9] : "", sizeof(results[i].forbidden_to_play) - 1);
        strncpy(results[i].forbidden_due_to, values[10] ? values[10] : "", sizeof(results[i].forbidden_due_to) - 1);
        results[i].tutorial_flag = atoi(values[11]);
        results[i].trade_gold_total = atoi(values[12]);
        results[i].trade_gold_total_billion = atoi(values[13]);
        results[i].trade_gold_daily = atoi(values[14]);
        results[i].dungeon_map_pass_cnt = atoi(values[15]);
        results[i].dungeon_map_help_pass_cnt = atoi(values[16]);
        results[i].help_abuse_point = atoi(values[17]);
        results[i].chaos_point = atoi(values[18]);
        results[i].chaos_exp = atoi(values[19]);
        results[i].chaos_mode_count = atoi(values[20]);
        results[i].chaos_kill_count = atoi(values[21]);
        results[i].chaos_die_count = atoi(values[22]);
        strncpy(results[i].chaos_die_time, values[23] ? values[23] : "", sizeof(results[i].chaos_die_time) - 1);
        strncpy(results[i].chaos_kill_time, values[24] ? values[24] : "", sizeof(results[i].chaos_kill_time) - 1);
        results[i].assault_count = atoi(values[25]);
        results[i].luck_point = atoi(values[26]);
        results[i].dungeon_play_count = atoi(values[27]);
        results[i].help_abuse_ratio = atoi(values[28]);
        results[i].help_abuse_exp = atoi(values[29]);
        results[i].expert_job_exp = atoi(values[30]);
        results[i].fatigue_battery_charging = atoi(values[31]);
        strncpy(results[i].escalade_tutorial_flag, values[32] ? values[32] : "", sizeof(results[i].escalade_tutorial_flag) - 1);
        results[i].power_war_point = atoi(values[33]);
        results[i].power_war_assault_count = atoi(values[34]);
        results[i].power_war_assault_victory_count = atoi(values[35]);
        results[i].fatigue_grownup_buff = atoi(values[36]);
        results[i].village_prev = atoi(values[37]);
        strncpy(results[i].last_play_time_powerwar, values[38] ? values[38] : "", sizeof(results[i].last_play_time_powerwar) - 1);
        results[i].emotion = atoi(values[39]);
        results[i].add_slot_flag = atoi(values[40]);
        results[i].member_dungeon_flag = atoi(values[41]);
        results[i].open_flag = atoi(values[42]);
        results[i].member_bonus_fatigue = atoi(values[43]);
        strncpy(results[i].birthday_effect_time, values[44] ? values[44] : "", sizeof(results[i].birthday_effect_time) - 1);
        results[i].visible_flags = atoi(values[45]);
        results[i].add_equipslot_flag = atoi(values[46]);
        results[i].channel_equipslot_switch = atoi(values[47]);
        results[i].expand_equipslot_switch = atoi(values[48]);
        results[i].growth_power_reward = atoi(values[49]);
        strncpy(results[i].chaos_respon_time, values[50] ? values[50] : "", sizeof(results[i].chaos_respon_time) - 1);
        results[i].last_play_dungeon_index = atoi(values[51]);
        results[i].total_play_time = atoi(values[52]);

        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}

int CharacStat_GetTotalCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    DBQueryResult qresult;
    int ret;
    int count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_stat");

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    printf("[DEBUG GetTotalCount] ExecuteQuery returned: %d, row_count=%d\n", ret, qresult.row_count);

    if (ret > 0 && DBQueryResult_FetchRow(&qresult, values) > 0) {
        count = atoi(values[0]);
        printf("[DEBUG GetTotalCount] count from DB: %d\n", count);
    }

    DBQueryResult_Free(&qresult);
    return count;
}
