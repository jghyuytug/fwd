#ifndef LOG_GAME_CHANNEL_ORM_H
#define LOG_GAME_CHANNEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char gc_up_time[20];
    signed char gc_game;
    int gc_no;
    char gc_channel[33];
    short gc_ch_group;
    char gc_ip[65];
    short gc_new;
    short gc_now;
    short gc_out;
} LogGameChannel;

/* CRUD Operations */
int LogGameChannel_Add(DBConnectionManager* manager, const LogGameChannel* record);
int LogGameChannel_Get(DBConnectionManager* manager, const char* gc_up_time, signed char gc_game, int gc_no, LogGameChannel* record);
int LogGameChannel_Update(DBConnectionManager* manager, const LogGameChannel* record);
int LogGameChannel_Delete(DBConnectionManager* manager, const char* gc_up_time, signed char gc_game, int gc_no);
int LogGameChannel_Exists(DBConnectionManager* manager, const char* gc_up_time, signed char gc_game, int gc_no);
int LogGameChannel_GetAll(DBConnectionManager* manager, LogGameChannel* records, int max_count, int* actual_count);

#endif /* LOG_GAME_CHANNEL_ORM_H */