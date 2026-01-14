#ifndef MOBILE_AUTH_REWARD_TW_ORM_H
#define MOBILE_AUTH_REWARD_TW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_date[20];
} MobileAuthRewardTw;

/* CRUD Operations */
int MobileAuthRewardTw_Add(DBConnectionManager* manager, const MobileAuthRewardTw* record);
int MobileAuthRewardTw_Get(DBConnectionManager* manager, int m_id, MobileAuthRewardTw* record);
int MobileAuthRewardTw_Update(DBConnectionManager* manager, const MobileAuthRewardTw* record);
int MobileAuthRewardTw_Delete(DBConnectionManager* manager, int m_id);
int MobileAuthRewardTw_Exists(DBConnectionManager* manager, int m_id);
int MobileAuthRewardTw_GetAll(DBConnectionManager* manager, MobileAuthRewardTw* records, int max_count, int* actual_count);

#endif /* MOBILE_AUTH_REWARD_TW_ORM_H */