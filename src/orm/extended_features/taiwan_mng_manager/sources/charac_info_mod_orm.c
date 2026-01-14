#include "charac_info_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacInfoMod_Add(DBConnectionManager* manager, const CharacInfoMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_info_mod (server_info, charac_no, lev_old, exp_old, fatigue_old, max_fatigue_old, premium_fatigue_old, max_premium_fatigue_old, money_old, coin_old, event_coin_old, pay_coin_old, lev_new, exp_new, fatigue_new, max_fatigue_new, premium_fatigue_new, max_premium_fatigue_new, money_new, coin_new, event_coin_new, pay_coin_new, MNG_user_id, work_time, work_type, memo, win_point_old, pvp_point_old, pvp_grade_old, win_old, lose_old, help_abuse_ratio_old, help_abuse_exp_old, power_war_point_old, win_point_new, pvp_point_new, pvp_grade_new, win_new, lose_new, help_abuse_ratio_new, help_abuse_exp_new, power_war_point_new) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->server_info, record->charac_no, record->lev_old, record->exp_old, record->fatigue_old, record->max_fatigue_old, record->premium_fatigue_old, record->max_premium_fatigue_old, record->money_old, record->coin_old, record->event_coin_old, record->pay_coin_old, record->lev_new, record->exp_new, record->fatigue_new, record->max_fatigue_new, record->premium_fatigue_new, record->max_premium_fatigue_new, record->money_new, record->coin_new, record->event_coin_new, record->pay_coin_new, record->MNG_user_id, record->work_time, record->work_type, record->memo, record->win_point_old, record->pvp_point_old, record->pvp_grade_old, record->win_old, record->lose_old, record->help_abuse_ratio_old, record->help_abuse_exp_old, record->power_war_point_old, record->win_point_new, record->pvp_point_new, record->pvp_grade_new, record->win_new, record->lose_new, record->help_abuse_ratio_new, record->help_abuse_exp_new, record->power_war_point_new);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacInfoMod_Get(DBConnectionManager* manager, int id, CharacInfoMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[43];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, lev_old, exp_old, fatigue_old, max_fatigue_old, premium_fatigue_old, max_premium_fatigue_old, money_old, coin_old, event_coin_old, pay_coin_old, lev_new, exp_new, fatigue_new, max_fatigue_new, premium_fatigue_new, max_premium_fatigue_new, money_new, coin_new, event_coin_new, pay_coin_new, MNG_user_id, work_time, work_type, memo, win_point_old, pvp_point_old, pvp_grade_old, win_old, lose_old, help_abuse_ratio_old, help_abuse_exp_old, power_war_point_old, win_point_new, pvp_point_new, pvp_grade_new, win_new, lose_new, help_abuse_ratio_new, help_abuse_exp_new, power_war_point_new FROM charac_info_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacInfoMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->lev_old = row[3] ? atoi(row[3]) : 0;
    record->exp_old = row[4] ? atoi(row[4]) : 0;
    record->fatigue_old = row[5] ? atoi(row[5]) : 0;
    record->max_fatigue_old = row[6] ? atoi(row[6]) : 0;
    record->premium_fatigue_old = row[7] ? atoi(row[7]) : 0;
    record->max_premium_fatigue_old = row[8] ? atoi(row[8]) : 0;
    record->money_old = row[9] ? atoi(row[9]) : 0;
    record->coin_old = row[10] ? atoi(row[10]) : 0;
    record->event_coin_old = row[11] ? atoi(row[11]) : 0;
    record->pay_coin_old = row[12] ? atoi(row[12]) : 0;
    record->lev_new = row[13] ? atoi(row[13]) : 0;
    record->exp_new = row[14] ? atoi(row[14]) : 0;
    record->fatigue_new = row[15] ? atoi(row[15]) : 0;
    record->max_fatigue_new = row[16] ? atoi(row[16]) : 0;
    record->premium_fatigue_new = row[17] ? atoi(row[17]) : 0;
    record->max_premium_fatigue_new = row[18] ? atoi(row[18]) : 0;
    record->money_new = row[19] ? atoi(row[19]) : 0;
    record->coin_new = row[20] ? atoi(row[20]) : 0;
    record->event_coin_new = row[21] ? atoi(row[21]) : 0;
    record->pay_coin_new = row[22] ? atoi(row[22]) : 0;
    if (row[23]) strncpy(record->MNG_user_id, row[23], sizeof(record->MNG_user_id) - 1);
    if (row[24]) strncpy(record->work_time, row[24], sizeof(record->work_time) - 1);
    record->work_type = row[25] ? row[25][0] : '\0';
    if (row[26]) strncpy(record->memo, row[26], sizeof(record->memo) - 1);
    record->win_point_old = row[27] ? atoi(row[27]) : 0;
    record->pvp_point_old = row[28] ? atoi(row[28]) : 0;
    record->pvp_grade_old = row[29] ? atoi(row[29]) : 0;
    record->win_old = row[30] ? atoi(row[30]) : 0;
    record->lose_old = row[31] ? atoi(row[31]) : 0;
    record->help_abuse_ratio_old = row[32] ? atoi(row[32]) : 0;
    record->help_abuse_exp_old = row[33] ? atoi(row[33]) : 0;
    record->power_war_point_old = row[34] ? atoi(row[34]) : 0;
    record->win_point_new = row[35] ? atoi(row[35]) : 0;
    record->pvp_point_new = row[36] ? atoi(row[36]) : 0;
    record->pvp_grade_new = row[37] ? atoi(row[37]) : 0;
    record->win_new = row[38] ? atoi(row[38]) : 0;
    record->lose_new = row[39] ? atoi(row[39]) : 0;
    record->help_abuse_ratio_new = row[40] ? atoi(row[40]) : 0;
    record->help_abuse_exp_new = row[41] ? atoi(row[41]) : 0;
    record->power_war_point_new = row[42] ? atoi(row[42]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacInfoMod_GetAll(DBConnectionManager* manager, CharacInfoMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[43];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, lev_old, exp_old, fatigue_old, max_fatigue_old, premium_fatigue_old, max_premium_fatigue_old, money_old, coin_old, event_coin_old, pay_coin_old, lev_new, exp_new, fatigue_new, max_fatigue_new, premium_fatigue_new, max_premium_fatigue_new, money_new, coin_new, event_coin_new, pay_coin_new, MNG_user_id, work_time, work_type, memo, win_point_old, pvp_point_old, pvp_grade_old, win_old, lose_old, help_abuse_ratio_old, help_abuse_exp_old, power_war_point_old, win_point_new, pvp_point_new, pvp_grade_new, win_new, lose_new, help_abuse_ratio_new, help_abuse_exp_new, power_war_point_new FROM charac_info_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacInfoMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].lev_old = row[3] ? atoi(row[3]) : 0;
        records[count].exp_old = row[4] ? atoi(row[4]) : 0;
        records[count].fatigue_old = row[5] ? atoi(row[5]) : 0;
        records[count].max_fatigue_old = row[6] ? atoi(row[6]) : 0;
        records[count].premium_fatigue_old = row[7] ? atoi(row[7]) : 0;
        records[count].max_premium_fatigue_old = row[8] ? atoi(row[8]) : 0;
        records[count].money_old = row[9] ? atoi(row[9]) : 0;
        records[count].coin_old = row[10] ? atoi(row[10]) : 0;
        records[count].event_coin_old = row[11] ? atoi(row[11]) : 0;
        records[count].pay_coin_old = row[12] ? atoi(row[12]) : 0;
        records[count].lev_new = row[13] ? atoi(row[13]) : 0;
        records[count].exp_new = row[14] ? atoi(row[14]) : 0;
        records[count].fatigue_new = row[15] ? atoi(row[15]) : 0;
        records[count].max_fatigue_new = row[16] ? atoi(row[16]) : 0;
        records[count].premium_fatigue_new = row[17] ? atoi(row[17]) : 0;
        records[count].max_premium_fatigue_new = row[18] ? atoi(row[18]) : 0;
        records[count].money_new = row[19] ? atoi(row[19]) : 0;
        records[count].coin_new = row[20] ? atoi(row[20]) : 0;
        records[count].event_coin_new = row[21] ? atoi(row[21]) : 0;
        records[count].pay_coin_new = row[22] ? atoi(row[22]) : 0;
        if (row[23]) strncpy(records[count].MNG_user_id, row[23], sizeof(records[count].MNG_user_id) - 1);
        if (row[24]) strncpy(records[count].work_time, row[24], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[25] ? row[25][0] : '\0';
        if (row[26]) strncpy(records[count].memo, row[26], sizeof(records[count].memo) - 1);
        records[count].win_point_old = row[27] ? atoi(row[27]) : 0;
        records[count].pvp_point_old = row[28] ? atoi(row[28]) : 0;
        records[count].pvp_grade_old = row[29] ? atoi(row[29]) : 0;
        records[count].win_old = row[30] ? atoi(row[30]) : 0;
        records[count].lose_old = row[31] ? atoi(row[31]) : 0;
        records[count].help_abuse_ratio_old = row[32] ? atoi(row[32]) : 0;
        records[count].help_abuse_exp_old = row[33] ? atoi(row[33]) : 0;
        records[count].power_war_point_old = row[34] ? atoi(row[34]) : 0;
        records[count].win_point_new = row[35] ? atoi(row[35]) : 0;
        records[count].pvp_point_new = row[36] ? atoi(row[36]) : 0;
        records[count].pvp_grade_new = row[37] ? atoi(row[37]) : 0;
        records[count].win_new = row[38] ? atoi(row[38]) : 0;
        records[count].lose_new = row[39] ? atoi(row[39]) : 0;
        records[count].help_abuse_ratio_new = row[40] ? atoi(row[40]) : 0;
        records[count].help_abuse_exp_new = row[41] ? atoi(row[41]) : 0;
        records[count].power_war_point_new = row[42] ? atoi(row[42]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
