#ifndef EVENT_ACCOUNT_FATIGUE_REWARD_ORM_H
#define EVENT_ACCOUNT_FATIGUE_REWARD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
} EventAccountFatigueReward;

/* CRUD Operations */
int EventAccountFatigueReward_Add(DBConnectionManager* manager, const EventAccountFatigueReward* record);
int EventAccountFatigueReward_Get(DBConnectionManager* manager, int m_id, EventAccountFatigueReward* record);
int EventAccountFatigueReward_Update(DBConnectionManager* manager, const EventAccountFatigueReward* record);
int EventAccountFatigueReward_Delete(DBConnectionManager* manager, int m_id);
int EventAccountFatigueReward_Exists(DBConnectionManager* manager, int m_id);
int EventAccountFatigueReward_GetAll(DBConnectionManager* manager, EventAccountFatigueReward* records, int max_count, int* actual_count);

#endif /* EVENT_ACCOUNT_FATIGUE_REWARD_ORM_H */