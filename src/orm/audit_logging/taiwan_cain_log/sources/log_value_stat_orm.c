#include "log_value_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogValueStat_Add(DBConnectionManager* manager, const LogValueStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_value_stat (channel_no, occ_time, level, uv, drop_gold, drop_item, result_card_gold, result_card_item, store_item_buy, jar_item, disjoint_create, upgrade_faild_forced_disjoint, quest_reward, consume_store_item_buy, consume_upgrade_attempt, consume_upgrade_faild, consume_stamina_recovery, consume_quest_consume, consume_auction_commision, consume_item_disjoint, consume_item_repair, consume_item_use, consume_item_drop, consume_gold_drop, gold_card_item, consume_gold_card_price, deathtower_card_gold, deathtower_card_item, consume_qp_init) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->channel_no, record->occ_time, record->level, record->uv, record->drop_gold, record->drop_item, record->result_card_gold, record->result_card_item, record->store_item_buy, record->jar_item, record->disjoint_create, record->upgrade_faild_forced_disjoint, record->quest_reward, record->consume_store_item_buy, record->consume_upgrade_attempt, record->consume_upgrade_faild, record->consume_stamina_recovery, record->consume_quest_consume, record->consume_auction_commision, record->consume_item_disjoint, record->consume_item_repair, record->consume_item_use, record->consume_item_drop, record->consume_gold_drop, record->gold_card_item, record->consume_gold_card_price, record->deathtower_card_gold, record->deathtower_card_item, record->consume_qp_init);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogValueStat_Get(DBConnectionManager* manager, int channel_no, const char* occ_time, int level, LogValueStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_time, level, uv, drop_gold, drop_item, result_card_gold, result_card_item, store_item_buy, jar_item, disjoint_create, upgrade_faild_forced_disjoint, quest_reward, consume_store_item_buy, consume_upgrade_attempt, consume_upgrade_faild, consume_stamina_recovery, consume_quest_consume, consume_auction_commision, consume_item_disjoint, consume_item_repair, consume_item_use, consume_item_drop, consume_gold_drop, gold_card_item, consume_gold_card_price, deathtower_card_gold, deathtower_card_item, consume_qp_init FROM log_value_stat WHERE channel_no = %d AND occ_time = '%s' AND level = %d",
        channel_no, occ_time, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogValueStat));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->level = row[2] ? atoi(row[2]) : 0;
    record->uv = row[3] ? atoi(row[3]) : 0;
    record->drop_gold = row[4] ? atoi(row[4]) : 0;
    record->drop_item = row[5] ? atoi(row[5]) : 0;
    record->result_card_gold = row[6] ? atoi(row[6]) : 0;
    record->result_card_item = row[7] ? atoi(row[7]) : 0;
    record->store_item_buy = row[8] ? atoi(row[8]) : 0;
    record->jar_item = row[9] ? atoi(row[9]) : 0;
    record->disjoint_create = row[10] ? atoi(row[10]) : 0;
    record->upgrade_faild_forced_disjoint = row[11] ? atoi(row[11]) : 0;
    record->quest_reward = row[12] ? atoi(row[12]) : 0;
    record->consume_store_item_buy = row[13] ? atoi(row[13]) : 0;
    record->consume_upgrade_attempt = row[14] ? atoi(row[14]) : 0;
    record->consume_upgrade_faild = row[15] ? atoi(row[15]) : 0;
    record->consume_stamina_recovery = row[16] ? atoi(row[16]) : 0;
    record->consume_quest_consume = row[17] ? atoi(row[17]) : 0;
    record->consume_auction_commision = row[18] ? atoi(row[18]) : 0;
    record->consume_item_disjoint = row[19] ? atoi(row[19]) : 0;
    record->consume_item_repair = row[20] ? atoi(row[20]) : 0;
    record->consume_item_use = row[21] ? atoi(row[21]) : 0;
    record->consume_item_drop = row[22] ? atoi(row[22]) : 0;
    record->consume_gold_drop = row[23] ? atoi(row[23]) : 0;
    record->gold_card_item = row[24] ? atoi(row[24]) : 0;
    record->consume_gold_card_price = row[25] ? atoi(row[25]) : 0;
    record->deathtower_card_gold = row[26] ? atoi(row[26]) : 0;
    record->deathtower_card_item = row[27] ? atoi(row[27]) : 0;
    record->consume_qp_init = row[28] ? atoi(row[28]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogValueStat_GetAll(DBConnectionManager* manager, LogValueStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_time, level, uv, drop_gold, drop_item, result_card_gold, result_card_item, store_item_buy, jar_item, disjoint_create, upgrade_faild_forced_disjoint, quest_reward, consume_store_item_buy, consume_upgrade_attempt, consume_upgrade_faild, consume_stamina_recovery, consume_quest_consume, consume_auction_commision, consume_item_disjoint, consume_item_repair, consume_item_use, consume_item_drop, consume_gold_drop, gold_card_item, consume_gold_card_price, deathtower_card_gold, deathtower_card_item, consume_qp_init FROM log_value_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogValueStat));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].level = row[2] ? atoi(row[2]) : 0;
        records[count].uv = row[3] ? atoi(row[3]) : 0;
        records[count].drop_gold = row[4] ? atoi(row[4]) : 0;
        records[count].drop_item = row[5] ? atoi(row[5]) : 0;
        records[count].result_card_gold = row[6] ? atoi(row[6]) : 0;
        records[count].result_card_item = row[7] ? atoi(row[7]) : 0;
        records[count].store_item_buy = row[8] ? atoi(row[8]) : 0;
        records[count].jar_item = row[9] ? atoi(row[9]) : 0;
        records[count].disjoint_create = row[10] ? atoi(row[10]) : 0;
        records[count].upgrade_faild_forced_disjoint = row[11] ? atoi(row[11]) : 0;
        records[count].quest_reward = row[12] ? atoi(row[12]) : 0;
        records[count].consume_store_item_buy = row[13] ? atoi(row[13]) : 0;
        records[count].consume_upgrade_attempt = row[14] ? atoi(row[14]) : 0;
        records[count].consume_upgrade_faild = row[15] ? atoi(row[15]) : 0;
        records[count].consume_stamina_recovery = row[16] ? atoi(row[16]) : 0;
        records[count].consume_quest_consume = row[17] ? atoi(row[17]) : 0;
        records[count].consume_auction_commision = row[18] ? atoi(row[18]) : 0;
        records[count].consume_item_disjoint = row[19] ? atoi(row[19]) : 0;
        records[count].consume_item_repair = row[20] ? atoi(row[20]) : 0;
        records[count].consume_item_use = row[21] ? atoi(row[21]) : 0;
        records[count].consume_item_drop = row[22] ? atoi(row[22]) : 0;
        records[count].consume_gold_drop = row[23] ? atoi(row[23]) : 0;
        records[count].gold_card_item = row[24] ? atoi(row[24]) : 0;
        records[count].consume_gold_card_price = row[25] ? atoi(row[25]) : 0;
        records[count].deathtower_card_gold = row[26] ? atoi(row[26]) : 0;
        records[count].deathtower_card_item = row[27] ? atoi(row[27]) : 0;
        records[count].consume_qp_init = row[28] ? atoi(row[28]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
