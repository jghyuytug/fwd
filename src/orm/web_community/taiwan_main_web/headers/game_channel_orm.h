#ifndef GAME_CHANNEL_ORM_H
#define GAME_CHANNEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gc_no;
    short gc_now;
    char gc_ip[65];
    short gc_port;
    short gc_max;
    signed char gc_game;
    char gc_channel[33];
    short gc_ch_group;
    char gc_channeltype[2];
} GameChannel;

/* CRUD Operations */
int GameChannel_Add(DBConnectionManager* manager, const GameChannel* record);
int GameChannel_Get(DBConnectionManager* manager, int gc_no, GameChannel* record);
int GameChannel_Update(DBConnectionManager* manager, const GameChannel* record);
int GameChannel_Delete(DBConnectionManager* manager, int gc_no);
int GameChannel_Exists(DBConnectionManager* manager, int gc_no);
int GameChannel_GetAll(DBConnectionManager* manager, GameChannel* records, int max_count, int* actual_count);

#endif /* GAME_CHANNEL_ORM_H */