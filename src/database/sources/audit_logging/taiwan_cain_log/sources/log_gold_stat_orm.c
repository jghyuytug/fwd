#include "log_gold_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogGoldStat_Add(DBConnectionManager* manager, const LogGoldStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_gold_stat (occ_time, channel_no, level, dungeon_drop, result_card, sell_store, quest_reward, death_tower_reward, illusion_tower_reward, war_area_drop, member_tax, sell_auction, fail_sell_auction, fail_bidding_auction, buy_store, stamina_recovery, repair_item, private_store_commission, gold_card, auction_commission, gold_drop, upgrade, quest_use, mail_commission, punish_user, restrict_trade, guild_level_up, guild_skill, guild_mail, item_compound, auction_bidding, war_area_enter, assault_gold, blood_dungeon_reward, power_dungeon_drop, power_dungeon_result_card, blood_dungeon_enter, blood_dungeon_lotto, buy_cerashop, upgrade_guild_agit, upgrade_guild_cargo, break_away_reward, link_charac_bonus, ultimate_dungeon_reward, guild_fund, guild_fund_dungeon, quest_shop_init_cost, unseal, lottery, amplify, roi_regen) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->channel_no, record->level, record->dungeon_drop, record->result_card, record->sell_store, record->quest_reward, record->death_tower_reward, record->illusion_tower_reward, record->war_area_drop, record->member_tax, record->sell_auction, record->fail_sell_auction, record->fail_bidding_auction, record->buy_store, record->stamina_recovery, record->repair_item, record->private_store_commission, record->gold_card, record->auction_commission, record->gold_drop, record->upgrade, record->quest_use, record->mail_commission, record->punish_user, record->restrict_trade, record->guild_level_up, record->guild_skill, record->guild_mail, record->item_compound, record->auction_bidding, record->war_area_enter, record->assault_gold, record->blood_dungeon_reward, record->power_dungeon_drop, record->power_dungeon_result_card, record->blood_dungeon_enter, record->blood_dungeon_lotto, record->buy_cerashop, record->upgrade_guild_agit, record->upgrade_guild_cargo, record->break_away_reward, record->link_charac_bonus, record->ultimate_dungeon_reward, record->guild_fund, record->guild_fund_dungeon, record->quest_shop_init_cost, record->unseal, record->lottery, record->amplify, record->roi_regen);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGoldStat_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int level, LogGoldStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[51];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, level, dungeon_drop, result_card, sell_store, quest_reward, death_tower_reward, illusion_tower_reward, war_area_drop, member_tax, sell_auction, fail_sell_auction, fail_bidding_auction, buy_store, stamina_recovery, repair_item, private_store_commission, gold_card, auction_commission, gold_drop, upgrade, quest_use, mail_commission, punish_user, restrict_trade, guild_level_up, guild_skill, guild_mail, item_compound, auction_bidding, war_area_enter, assault_gold, blood_dungeon_reward, power_dungeon_drop, power_dungeon_result_card, blood_dungeon_enter, blood_dungeon_lotto, buy_cerashop, upgrade_guild_agit, upgrade_guild_cargo, break_away_reward, link_charac_bonus, ultimate_dungeon_reward, guild_fund, guild_fund_dungeon, quest_shop_init_cost, unseal, lottery, amplify, roi_regen FROM log_gold_stat WHERE occ_time = '%s' AND channel_no = %d AND level = %d",
        occ_time, channel_no, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogGoldStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->level = row[2] ? atoi(row[2]) : 0;
    record->dungeon_drop = row[3] ? atoi(row[3]) : 0;
    record->result_card = row[4] ? atoi(row[4]) : 0;
    record->sell_store = row[5] ? atoi(row[5]) : 0;
    record->quest_reward = row[6] ? atoi(row[6]) : 0;
    record->death_tower_reward = row[7] ? atoi(row[7]) : 0;
    record->illusion_tower_reward = row[8] ? atoi(row[8]) : 0;
    record->war_area_drop = row[9] ? atoi(row[9]) : 0;
    record->member_tax = row[10] ? atoi(row[10]) : 0;
    record->sell_auction = row[11] ? atoi(row[11]) : 0;
    record->fail_sell_auction = row[12] ? atoi(row[12]) : 0;
    record->fail_bidding_auction = row[13] ? atoi(row[13]) : 0;
    record->buy_store = row[14] ? atoi(row[14]) : 0;
    record->stamina_recovery = row[15] ? atoi(row[15]) : 0;
    record->repair_item = row[16] ? atoi(row[16]) : 0;
    record->private_store_commission = row[17] ? atoi(row[17]) : 0;
    record->gold_card = row[18] ? atoi(row[18]) : 0;
    record->auction_commission = row[19] ? atoi(row[19]) : 0;
    record->gold_drop = row[20] ? atoi(row[20]) : 0;
    record->upgrade = row[21] ? atoi(row[21]) : 0;
    record->quest_use = row[22] ? atoi(row[22]) : 0;
    record->mail_commission = row[23] ? atoi(row[23]) : 0;
    record->punish_user = row[24] ? atoi(row[24]) : 0;
    record->restrict_trade = row[25] ? atoi(row[25]) : 0;
    record->guild_level_up = row[26] ? atoi(row[26]) : 0;
    record->guild_skill = row[27] ? atoi(row[27]) : 0;
    record->guild_mail = row[28] ? atoi(row[28]) : 0;
    record->item_compound = row[29] ? atoi(row[29]) : 0;
    record->auction_bidding = row[30] ? atoi(row[30]) : 0;
    record->war_area_enter = row[31] ? atoi(row[31]) : 0;
    record->assault_gold = row[32] ? atoi(row[32]) : 0;
    record->blood_dungeon_reward = row[33] ? atoi(row[33]) : 0;
    record->power_dungeon_drop = row[34] ? atoi(row[34]) : 0;
    record->power_dungeon_result_card = row[35] ? atoi(row[35]) : 0;
    record->blood_dungeon_enter = row[36] ? atoi(row[36]) : 0;
    record->blood_dungeon_lotto = row[37] ? atoi(row[37]) : 0;
    record->buy_cerashop = row[38] ? atoi(row[38]) : 0;
    record->upgrade_guild_agit = row[39] ? atoi(row[39]) : 0;
    record->upgrade_guild_cargo = row[40] ? atoi(row[40]) : 0;
    record->break_away_reward = row[41] ? atoi(row[41]) : 0;
    record->link_charac_bonus = row[42] ? atoi(row[42]) : 0;
    record->ultimate_dungeon_reward = row[43] ? atoi(row[43]) : 0;
    record->guild_fund = row[44] ? atoi(row[44]) : 0;
    record->guild_fund_dungeon = row[45] ? atoi(row[45]) : 0;
    record->quest_shop_init_cost = row[46] ? atoi(row[46]) : 0;
    record->unseal = row[47] ? atoi(row[47]) : 0;
    record->lottery = row[48] ? atoi(row[48]) : 0;
    record->amplify = row[49] ? atoi(row[49]) : 0;
    record->roi_regen = row[50] ? atoi(row[50]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogGoldStat_GetAll(DBConnectionManager* manager, LogGoldStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[51];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, level, dungeon_drop, result_card, sell_store, quest_reward, death_tower_reward, illusion_tower_reward, war_area_drop, member_tax, sell_auction, fail_sell_auction, fail_bidding_auction, buy_store, stamina_recovery, repair_item, private_store_commission, gold_card, auction_commission, gold_drop, upgrade, quest_use, mail_commission, punish_user, restrict_trade, guild_level_up, guild_skill, guild_mail, item_compound, auction_bidding, war_area_enter, assault_gold, blood_dungeon_reward, power_dungeon_drop, power_dungeon_result_card, blood_dungeon_enter, blood_dungeon_lotto, buy_cerashop, upgrade_guild_agit, upgrade_guild_cargo, break_away_reward, link_charac_bonus, ultimate_dungeon_reward, guild_fund, guild_fund_dungeon, quest_shop_init_cost, unseal, lottery, amplify, roi_regen FROM log_gold_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGoldStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].level = row[2] ? atoi(row[2]) : 0;
        records[count].dungeon_drop = row[3] ? atoi(row[3]) : 0;
        records[count].result_card = row[4] ? atoi(row[4]) : 0;
        records[count].sell_store = row[5] ? atoi(row[5]) : 0;
        records[count].quest_reward = row[6] ? atoi(row[6]) : 0;
        records[count].death_tower_reward = row[7] ? atoi(row[7]) : 0;
        records[count].illusion_tower_reward = row[8] ? atoi(row[8]) : 0;
        records[count].war_area_drop = row[9] ? atoi(row[9]) : 0;
        records[count].member_tax = row[10] ? atoi(row[10]) : 0;
        records[count].sell_auction = row[11] ? atoi(row[11]) : 0;
        records[count].fail_sell_auction = row[12] ? atoi(row[12]) : 0;
        records[count].fail_bidding_auction = row[13] ? atoi(row[13]) : 0;
        records[count].buy_store = row[14] ? atoi(row[14]) : 0;
        records[count].stamina_recovery = row[15] ? atoi(row[15]) : 0;
        records[count].repair_item = row[16] ? atoi(row[16]) : 0;
        records[count].private_store_commission = row[17] ? atoi(row[17]) : 0;
        records[count].gold_card = row[18] ? atoi(row[18]) : 0;
        records[count].auction_commission = row[19] ? atoi(row[19]) : 0;
        records[count].gold_drop = row[20] ? atoi(row[20]) : 0;
        records[count].upgrade = row[21] ? atoi(row[21]) : 0;
        records[count].quest_use = row[22] ? atoi(row[22]) : 0;
        records[count].mail_commission = row[23] ? atoi(row[23]) : 0;
        records[count].punish_user = row[24] ? atoi(row[24]) : 0;
        records[count].restrict_trade = row[25] ? atoi(row[25]) : 0;
        records[count].guild_level_up = row[26] ? atoi(row[26]) : 0;
        records[count].guild_skill = row[27] ? atoi(row[27]) : 0;
        records[count].guild_mail = row[28] ? atoi(row[28]) : 0;
        records[count].item_compound = row[29] ? atoi(row[29]) : 0;
        records[count].auction_bidding = row[30] ? atoi(row[30]) : 0;
        records[count].war_area_enter = row[31] ? atoi(row[31]) : 0;
        records[count].assault_gold = row[32] ? atoi(row[32]) : 0;
        records[count].blood_dungeon_reward = row[33] ? atoi(row[33]) : 0;
        records[count].power_dungeon_drop = row[34] ? atoi(row[34]) : 0;
        records[count].power_dungeon_result_card = row[35] ? atoi(row[35]) : 0;
        records[count].blood_dungeon_enter = row[36] ? atoi(row[36]) : 0;
        records[count].blood_dungeon_lotto = row[37] ? atoi(row[37]) : 0;
        records[count].buy_cerashop = row[38] ? atoi(row[38]) : 0;
        records[count].upgrade_guild_agit = row[39] ? atoi(row[39]) : 0;
        records[count].upgrade_guild_cargo = row[40] ? atoi(row[40]) : 0;
        records[count].break_away_reward = row[41] ? atoi(row[41]) : 0;
        records[count].link_charac_bonus = row[42] ? atoi(row[42]) : 0;
        records[count].ultimate_dungeon_reward = row[43] ? atoi(row[43]) : 0;
        records[count].guild_fund = row[44] ? atoi(row[44]) : 0;
        records[count].guild_fund_dungeon = row[45] ? atoi(row[45]) : 0;
        records[count].quest_shop_init_cost = row[46] ? atoi(row[46]) : 0;
        records[count].unseal = row[47] ? atoi(row[47]) : 0;
        records[count].lottery = row[48] ? atoi(row[48]) : 0;
        records[count].amplify = row[49] ? atoi(row[49]) : 0;
        records[count].roi_regen = row[50] ? atoi(row[50]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
