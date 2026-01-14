#ifndef GUILD_INFO_ORM_H
#define GUILD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    signed char server_id;
    char guild_name[41];
    int master_id;
    int master_no;
    char master_name[21];
    char guild_url[41];
    signed char guild_icon;
    char create_time[20];
    int lev;
    signed char ability;
    signed char expire_flag;
    char expire_time[20];
    char member_secede_time[20];
    int member_count;
    signed char recommend_flag;
    char recommend_time[20];
    int guild_point;
    int guild_point_acc;
    int guild_point_prev;
    int guild_rank;
    int guild_war_point;
    short final_entry;
    short final_win;
    signed char guild_icon_auth;
    int guild_exp;
} GuildInfo;

/* CRUD Operations */
int GuildInfo_Add(DBConnectionManager* manager, const GuildInfo* record);
int GuildInfo_Get(DBConnectionManager* manager, int guild_id, GuildInfo* record);
int GuildInfo_Update(DBConnectionManager* manager, const GuildInfo* record);
int GuildInfo_Delete(DBConnectionManager* manager, int guild_id);
int GuildInfo_Exists(DBConnectionManager* manager, int guild_id);
int GuildInfo_GetAll(DBConnectionManager* manager, GuildInfo* records, int max_count, int* actual_count);

#endif /* GUILD_INFO_ORM_H */