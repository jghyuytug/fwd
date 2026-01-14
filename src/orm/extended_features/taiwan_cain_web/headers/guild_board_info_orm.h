#ifndef GUILD_BOARD_INFO_ORM_H
#define GUILD_BOARD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int board_id;
    int guild_id;
    signed char board_type;
    char board_name[51];
    char create_day[20];
    signed char seq;
    char modify_day[20];
    char delete_day[20];
    signed char list_auth;
    signed char content_auth;
    signed char write_auth;
    signed char comment_flag;
} GuildBoardInfo;

/* CRUD Operations */
int GuildBoardInfo_Add(DBConnectionManager* manager, const GuildBoardInfo* record);
int GuildBoardInfo_Get(DBConnectionManager* manager, int board_id, GuildBoardInfo* record);
int GuildBoardInfo_Update(DBConnectionManager* manager, const GuildBoardInfo* record);
int GuildBoardInfo_Delete(DBConnectionManager* manager, int board_id);
int GuildBoardInfo_Exists(DBConnectionManager* manager, int board_id);
int GuildBoardInfo_GetAll(DBConnectionManager* manager, GuildBoardInfo* records, int max_count, int* actual_count);

#endif /* GUILD_BOARD_INFO_ORM_H */