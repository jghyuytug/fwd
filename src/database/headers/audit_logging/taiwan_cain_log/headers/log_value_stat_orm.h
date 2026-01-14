#ifndef LOG_VALUE_STAT_ORM_H
#define LOG_VALUE_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    char occ_time[20];
    int level;
    int uv;
    int drop_gold;
    int drop_item;
    int result_card_gold;
    int result_card_item;
    int store_item_buy;
    int jar_item;
    int disjoint_create;
    int upgrade_faild_forced_disjoint;
    int quest_reward;
    int consume_store_item_buy;
    int consume_upgrade_attempt;
    int consume_upgrade_faild;
    int consume_stamina_recovery;
    int consume_quest_consume;
    int consume_auction_commision;
    int consume_item_disjoint;
    int consume_item_repair;
    int consume_item_use;
    int consume_item_drop;
    int consume_gold_drop;
    int gold_card_item;
    int consume_gold_card_price;
    int deathtower_card_gold;
    int deathtower_card_item;
    int consume_qp_init;
} LogValueStat;

/* CRUD Operations */
int LogValueStat_Add(DBConnectionManager* manager, const LogValueStat* record);
int LogValueStat_Get(DBConnectionManager* manager, int channel_no, const char* occ_time, int level, LogValueStat* record);
int LogValueStat_Update(DBConnectionManager* manager, const LogValueStat* record);
int LogValueStat_Delete(DBConnectionManager* manager, int channel_no, const char* occ_time, int level);
int LogValueStat_Exists(DBConnectionManager* manager, int channel_no, const char* occ_time, int level);
int LogValueStat_GetAll(DBConnectionManager* manager, LogValueStat* records, int max_count, int* actual_count);

#endif /* LOG_VALUE_STAT_ORM_H */