#ifndef GUILD_INFO_ORM_H
#define GUILD_INFO_ORM_H

#include "database/headers/db_connection_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int guild_id;
    int server_id;
    char guild_name[41];
    int master_id;
    int master_no;
    char master_name[21];
    char guild_url[256];
    int guild_icon;
    char create_time[20];
    int lev;
    int ability;
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
    int final_entry;
    int final_win;
    int guild_icon_auth;
    int guild_exp;
    int power_side;
    int guild_agit_flag;
    char lev_up_time[20];
    char power_secede_time[20];
    int power_war_point;
    int power_join_count;
    int guild_fund;
    char guild_notice[256];
} GuildInfo;

/* Basic CRUD operations */
int GuildInfo_Get(DBConnectionManager* manager, int guild_id, GuildInfo* info);
int GuildInfo_Add(DBConnectionManager* manager, const GuildInfo* info);
int GuildInfo_Update(DBConnectionManager* manager, const GuildInfo* info);
int GuildInfo_Delete(DBConnectionManager* manager, int guild_id);

/* Extended operations */
int GuildInfo_AddEx(DBConnectionManager* manager, const GuildInfo* info, unsigned long long* out_guild_id);
int GuildInfo_UpdateMemberCount(DBConnectionManager* manager, int guild_id, int delta);
int GuildInfo_SetExpireFlag(DBConnectionManager* manager, int guild_id, int flag);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_INFO_ORM_H */
