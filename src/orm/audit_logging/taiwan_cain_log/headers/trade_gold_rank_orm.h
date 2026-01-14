#ifndef TRADE_GOLD_RANK_ORM_H
#define TRADE_GOLD_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int m_id;
    int gold;
} TradeGoldRank;

/* CRUD Operations */
int TradeGoldRank_Add(DBConnectionManager* manager, const TradeGoldRank* record);
int TradeGoldRank_Get(DBConnectionManager* manager, const char* occ_date, int m_id, TradeGoldRank* record);
int TradeGoldRank_Update(DBConnectionManager* manager, const TradeGoldRank* record);
int TradeGoldRank_Delete(DBConnectionManager* manager, const char* occ_date, int m_id);
int TradeGoldRank_Exists(DBConnectionManager* manager, const char* occ_date, int m_id);
int TradeGoldRank_GetAll(DBConnectionManager* manager, TradeGoldRank* records, int max_count, int* actual_count);

#endif /* TRADE_GOLD_RANK_ORM_H */