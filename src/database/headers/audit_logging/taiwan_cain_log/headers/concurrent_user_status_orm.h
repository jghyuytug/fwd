#ifndef CONCURRENT_USER_STATUS_ORM_H
#define CONCURRENT_USER_STATUS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char player_status;
    int dungeon_index;
    int channel_no;
    int occ_count;
} ConcurrentUserStatus;

/* CRUD Operations */
int ConcurrentUserStatus_Add(DBConnectionManager* manager, const ConcurrentUserStatus* record);
int ConcurrentUserStatus_Get(DBConnectionManager* manager, const char* occ_time, signed char player_status, int dungeon_index, int channel_no, ConcurrentUserStatus* record);
int ConcurrentUserStatus_Update(DBConnectionManager* manager, const ConcurrentUserStatus* record);
int ConcurrentUserStatus_Delete(DBConnectionManager* manager, const char* occ_time, signed char player_status, int dungeon_index, int channel_no);
int ConcurrentUserStatus_Exists(DBConnectionManager* manager, const char* occ_time, signed char player_status, int dungeon_index, int channel_no);
int ConcurrentUserStatus_GetAll(DBConnectionManager* manager, ConcurrentUserStatus* records, int max_count, int* actual_count);

#endif /* CONCURRENT_USER_STATUS_ORM_H */