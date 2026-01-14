#ifndef CHANNEL_LEV_STATUS_ORM_H
#define CHANNEL_LEV_STATUS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char channel_no;
    signed char lev;
    int occ_count;
} ChannelLevStatus;

/* CRUD Operations */
int ChannelLevStatus_Add(DBConnectionManager* manager, const ChannelLevStatus* record);
int ChannelLevStatus_Get(DBConnectionManager* manager, const char* occ_time, signed char channel_no, signed char lev, ChannelLevStatus* record);
int ChannelLevStatus_Update(DBConnectionManager* manager, const ChannelLevStatus* record);
int ChannelLevStatus_Delete(DBConnectionManager* manager, const char* occ_time, signed char channel_no, signed char lev);
int ChannelLevStatus_Exists(DBConnectionManager* manager, const char* occ_time, signed char channel_no, signed char lev);
int ChannelLevStatus_GetAll(DBConnectionManager* manager, ChannelLevStatus* records, int max_count, int* actual_count);

#endif /* CHANNEL_LEV_STATUS_ORM_H */