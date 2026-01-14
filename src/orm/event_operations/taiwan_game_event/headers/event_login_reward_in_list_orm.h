#ifndef EVENT_LOGIN_REWARD_IN_LIST_ORM_H
#define EVENT_LOGIN_REWARD_IN_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_date[20];
    signed char reward_flag;
} EventLoginRewardInList;

/* CRUD Operations */
int EventLoginRewardInList_Add(DBConnectionManager* manager, const EventLoginRewardInList* record);
int EventLoginRewardInList_Get(DBConnectionManager* manager, int m_id, EventLoginRewardInList* record);
int EventLoginRewardInList_Update(DBConnectionManager* manager, const EventLoginRewardInList* record);
int EventLoginRewardInList_Delete(DBConnectionManager* manager, int m_id);
int EventLoginRewardInList_Exists(DBConnectionManager* manager, int m_id);
int EventLoginRewardInList_GetAll(DBConnectionManager* manager, EventLoginRewardInList* records, int max_count, int* actual_count);

#endif /* EVENT_LOGIN_REWARD_IN_LIST_ORM_H */