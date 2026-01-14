#include "cleanup_constant_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CleanupConstant_Add(DBConnectionManager* manager, const CleanupConstant* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO cleanup_constant (limit_penalty, base1_penalty, base2_penalty, base3_penalty, base4_penalty, person_trade, person_shop_trade, auction_trade, mail_trade, mail_min_gold, abnormal_npc_trade, abnormal_user_trade, dungeon_clear, removal_dungeon_clear, penalty_user_trade, penalty_ghost_clear_n, penalty_ghost_clear_m, penalty_ghost_clear_l, pc_room_weight, hps_ip_weight, otm_weight_n, otm_weight_m, hack_weight, pvp_penalty, dungeon_clear_penalty, quest_clear_penalty, reduce_time_date, reduce_time_penalty) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->limit_penalty, record->base1_penalty, record->base2_penalty, record->base3_penalty, record->base4_penalty, record->person_trade, record->person_shop_trade, record->auction_trade, record->mail_trade, record->mail_min_gold, record->abnormal_npc_trade, record->abnormal_user_trade, record->dungeon_clear, record->removal_dungeon_clear, record->penalty_user_trade, record->penalty_ghost_clear_n, record->penalty_ghost_clear_m, record->penalty_ghost_clear_l, record->pc_room_weight, record->hps_ip_weight, record->otm_weight_n, record->otm_weight_m, record->hack_weight, record->pvp_penalty, record->dungeon_clear_penalty, record->quest_clear_penalty, record->reduce_time_date, record->reduce_time_penalty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CleanupConstant_GetAll(DBConnectionManager* manager, CleanupConstant* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[28];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT limit_penalty, base1_penalty, base2_penalty, base3_penalty, base4_penalty, person_trade, person_shop_trade, auction_trade, mail_trade, mail_min_gold, abnormal_npc_trade, abnormal_user_trade, dungeon_clear, removal_dungeon_clear, penalty_user_trade, penalty_ghost_clear_n, penalty_ghost_clear_m, penalty_ghost_clear_l, pc_room_weight, hps_ip_weight, otm_weight_n, otm_weight_m, hack_weight, pvp_penalty, dungeon_clear_penalty, quest_clear_penalty, reduce_time_date, reduce_time_penalty FROM cleanup_constant");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CleanupConstant));
        records[count].limit_penalty = row[0] ? atoi(row[0]) : 0;
        records[count].base1_penalty = row[1] ? atoi(row[1]) : 0;
        records[count].base2_penalty = row[2] ? atoi(row[2]) : 0;
        records[count].base3_penalty = row[3] ? atoi(row[3]) : 0;
        records[count].base4_penalty = row[4] ? atoi(row[4]) : 0;
        records[count].person_trade = row[5] ? atoi(row[5]) : 0;
        records[count].person_shop_trade = row[6] ? atoi(row[6]) : 0;
        records[count].auction_trade = row[7] ? atoi(row[7]) : 0;
        records[count].mail_trade = row[8] ? atoi(row[8]) : 0;
        records[count].mail_min_gold = row[9] ? atoi(row[9]) : 0;
        records[count].abnormal_npc_trade = row[10] ? atoi(row[10]) : 0;
        records[count].abnormal_user_trade = row[11] ? atoi(row[11]) : 0;
        records[count].dungeon_clear = row[12] ? atoi(row[12]) : 0;
        records[count].removal_dungeon_clear = row[13] ? atoi(row[13]) : 0;
        records[count].penalty_user_trade = row[14] ? atoi(row[14]) : 0;
        records[count].penalty_ghost_clear_n = row[15] ? atoi(row[15]) : 0;
        records[count].penalty_ghost_clear_m = row[16] ? atoi(row[16]) : 0;
        records[count].penalty_ghost_clear_l = row[17] ? atoi(row[17]) : 0;
        records[count].pc_room_weight = row[18] ? atoi(row[18]) : 0;
        records[count].hps_ip_weight = row[19] ? atoi(row[19]) : 0;
        records[count].otm_weight_n = row[20] ? atoi(row[20]) : 0;
        records[count].otm_weight_m = row[21] ? atoi(row[21]) : 0;
        records[count].hack_weight = row[22] ? atoi(row[22]) : 0;
        records[count].pvp_penalty = row[23] ? atoi(row[23]) : 0;
        records[count].dungeon_clear_penalty = row[24] ? atoi(row[24]) : 0;
        records[count].quest_clear_penalty = row[25] ? atoi(row[25]) : 0;
        records[count].reduce_time_date = row[26] ? atoi(row[26]) : 0;
        records[count].reduce_time_penalty = row[27] ? atoi(row[27]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
