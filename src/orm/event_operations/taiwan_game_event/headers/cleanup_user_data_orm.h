#ifndef CLEANUP_USER_DATA_ORM_H
#define CLEANUP_USER_DATA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char penalty;
    signed char restriction_cnt;
    signed char cur_state;
    signed char user_trade_cnt;
    signed char npc_trade_cnt;
    signed char dungeon_clear_cnt;
    int last_clear_map_idx;
    signed char ghost_clear_cnt;
    char last_penalty_date[20];
    signed char other_penalty;
    char other_penalty_date[20];
} CleanupUserData;

/* CRUD Operations */
int CleanupUserData_Add(DBConnectionManager* manager, const CleanupUserData* record);
int CleanupUserData_Get(DBConnectionManager* manager, int m_id, CleanupUserData* record);
int CleanupUserData_Update(DBConnectionManager* manager, const CleanupUserData* record);
int CleanupUserData_Delete(DBConnectionManager* manager, int m_id);
int CleanupUserData_Exists(DBConnectionManager* manager, int m_id);
int CleanupUserData_GetAll(DBConnectionManager* manager, CleanupUserData* records, int max_count, int* actual_count);

#endif /* CLEANUP_USER_DATA_ORM_H */