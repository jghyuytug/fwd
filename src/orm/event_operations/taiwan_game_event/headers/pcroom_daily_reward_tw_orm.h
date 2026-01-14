#ifndef PCROOM_DAILY_REWARD_TW_ORM_H
#define PCROOM_DAILY_REWARD_TW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_date[20];
    signed char remain_reward_count;
} PcroomDailyRewardTw;

/* CRUD Operations */
int PcroomDailyRewardTw_Add(DBConnectionManager* manager, const PcroomDailyRewardTw* record);
int PcroomDailyRewardTw_Get(DBConnectionManager* manager, int m_id, PcroomDailyRewardTw* record);
int PcroomDailyRewardTw_Update(DBConnectionManager* manager, const PcroomDailyRewardTw* record);
int PcroomDailyRewardTw_Delete(DBConnectionManager* manager, int m_id);
int PcroomDailyRewardTw_Exists(DBConnectionManager* manager, int m_id);
int PcroomDailyRewardTw_GetAll(DBConnectionManager* manager, PcroomDailyRewardTw* records, int max_count, int* actual_count);

#endif /* PCROOM_DAILY_REWARD_TW_ORM_H */