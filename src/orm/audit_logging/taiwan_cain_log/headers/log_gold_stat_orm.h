#ifndef LOG_GOLD_STAT_ORM_H
#define LOG_GOLD_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int channel_no;
    int level;
    int dungeon_drop;
    int result_card;
    int sell_store;
    int quest_reward;
    int death_tower_reward;
    int illusion_tower_reward;
    int war_area_drop;
    int member_tax;
    int sell_auction;
    int fail_sell_auction;
    int fail_bidding_auction;
    int buy_store;
    int stamina_recovery;
    int repair_item;
    int private_store_commission;
    int gold_card;
    int auction_commission;
    int gold_drop;
    int upgrade;
    int quest_use;
    int mail_commission;
    int punish_user;
    int restrict_trade;
    int guild_level_up;
    int guild_skill;
    int guild_mail;
    int item_compound;
    int auction_bidding;
    int war_area_enter;
    int assault_gold;
    int blood_dungeon_reward;
    int power_dungeon_drop;
    int power_dungeon_result_card;
    int blood_dungeon_enter;
    int blood_dungeon_lotto;
    int buy_cerashop;
    int upgrade_guild_agit;
    int upgrade_guild_cargo;
    int break_away_reward;
    int link_charac_bonus;
    int ultimate_dungeon_reward;
    int guild_fund;
    int guild_fund_dungeon;
    int quest_shop_init_cost;
    int unseal;
    int lottery;
    int amplify;
    int roi_regen;
} LogGoldStat;

/* CRUD Operations */
int LogGoldStat_Add(DBConnectionManager* manager, const LogGoldStat* record);
int LogGoldStat_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int level, LogGoldStat* record);
int LogGoldStat_Update(DBConnectionManager* manager, const LogGoldStat* record);
int LogGoldStat_Delete(DBConnectionManager* manager, const char* occ_time, int channel_no, int level);
int LogGoldStat_Exists(DBConnectionManager* manager, const char* occ_time, int channel_no, int level);
int LogGoldStat_GetAll(DBConnectionManager* manager, LogGoldStat* records, int max_count, int* actual_count);

#endif /* LOG_GOLD_STAT_ORM_H */