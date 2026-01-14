#ifndef TRADE_GOLD_DAILY_ORM_H
#define TRADE_GOLD_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int gold;
    char occ_time[20];
} TradeGoldDaily;

/* CRUD Operations */
int TradeGoldDaily_Add(DBConnectionManager* manager, const TradeGoldDaily* record);
int TradeGoldDaily_Get(DBConnectionManager* manager, int m_id, TradeGoldDaily* record);
int TradeGoldDaily_Update(DBConnectionManager* manager, const TradeGoldDaily* record);
int TradeGoldDaily_Delete(DBConnectionManager* manager, int m_id);
int TradeGoldDaily_Exists(DBConnectionManager* manager, int m_id);
int TradeGoldDaily_GetAll(DBConnectionManager* manager, TradeGoldDaily* records, int max_count, int* actual_count);

#endif /* TRADE_GOLD_DAILY_ORM_H */