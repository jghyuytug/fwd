#ifndef EVENT_1306_ACCOUNT_REWARD_2ND_ORM_H
#define EVENT_1306_ACCOUNT_REWARD_2ND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    char occ_date[20];
} Event1306AccountReward2Nd;

/* CRUD Operations */
int Event1306AccountReward2Nd_Add(DBConnectionManager* manager, const Event1306AccountReward2Nd* record);
int Event1306AccountReward2Nd_Get(DBConnectionManager* manager, int m_id, const char* occ_date, Event1306AccountReward2Nd* record);
int Event1306AccountReward2Nd_Update(DBConnectionManager* manager, const Event1306AccountReward2Nd* record);
int Event1306AccountReward2Nd_Delete(DBConnectionManager* manager, int m_id, const char* occ_date);
int Event1306AccountReward2Nd_Exists(DBConnectionManager* manager, int m_id, const char* occ_date);
int Event1306AccountReward2Nd_GetAll(DBConnectionManager* manager, Event1306AccountReward2Nd* records, int max_count, int* actual_count);

#endif /* EVENT_1306_ACCOUNT_REWARD_2ND_ORM_H */