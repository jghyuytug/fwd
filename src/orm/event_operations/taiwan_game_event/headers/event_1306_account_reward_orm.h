#ifndef EVENT_1306_ACCOUNT_REWARD_ORM_H
#define EVENT_1306_ACCOUNT_REWARD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    char occ_date[20];
} Event1306AccountReward;

/* CRUD Operations */
int Event1306AccountReward_Add(DBConnectionManager* manager, const Event1306AccountReward* record);
int Event1306AccountReward_Get(DBConnectionManager* manager, int m_id, Event1306AccountReward* record);
int Event1306AccountReward_Update(DBConnectionManager* manager, const Event1306AccountReward* record);
int Event1306AccountReward_Delete(DBConnectionManager* manager, int m_id);
int Event1306AccountReward_Exists(DBConnectionManager* manager, int m_id);
int Event1306AccountReward_GetAll(DBConnectionManager* manager, Event1306AccountReward* records, int max_count, int* actual_count);

#endif /* EVENT_1306_ACCOUNT_REWARD_ORM_H */