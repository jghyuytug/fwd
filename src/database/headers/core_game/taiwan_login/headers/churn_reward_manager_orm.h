#ifndef CHURN_REWARD_MANAGER_ORM_H
#define CHURN_REWARD_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char min_day;       /* Minimum day (PK1) */
    unsigned char max_day;       /* Maximum day (PK2) */
    unsigned int min_val;        /* Minimum value (PK3) */
    unsigned int max_val;        /* Maximum value (PK4) */
    unsigned char quest_id;      /* Quest ID (PK5) */
    unsigned int item_id;        /* Item ID */
    unsigned int add_info;       /* Additional info */
    unsigned int luck_point;     /* Luck points */
} ChurnRewardManager;

/* CRUD Operations - All require 5 keys */
int ChurnRewardManager_Add(DBConnectionManager* manager, const ChurnRewardManager* reward);
int ChurnRewardManager_Get(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                            unsigned int min_val, unsigned int max_val, unsigned char quest_id,
                            ChurnRewardManager* reward);
int ChurnRewardManager_Delete(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                               unsigned int min_val, unsigned int max_val, unsigned char quest_id);
int ChurnRewardManager_Exists(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                               unsigned int min_val, unsigned int max_val, unsigned char quest_id);

/* Business Operations */
int ChurnRewardManager_GetByDayRange(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                                      ChurnRewardManager* rewards, int max_count, int* actual_count);
int ChurnRewardManager_GetByQuest(DBConnectionManager* manager, unsigned char quest_id,
                                   ChurnRewardManager* rewards, int max_count, int* actual_count);
int ChurnRewardManager_GetAll(DBConnectionManager* manager, ChurnRewardManager* rewards,
                               int max_count, int* actual_count);

/* Utility Functions */
void ChurnRewardManager_PrintInfo(const ChurnRewardManager* reward);

#endif /* CHURN_REWARD_MANAGER_ORM_H */
