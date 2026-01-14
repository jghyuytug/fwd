#ifndef GUILD_RANK_ORM_H
#define GUILD_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    int guild_id;
    char guild_name[41];
    short guild_Rank;
    int guild_point;
    int guild_acc_point;
    int guild_visit;
    int guild_acc_visit;
    short guild_member;
    short guild_acc_member;
    short guild_avg_lev;
} GuildRank;

/* CRUD Operations */
int GuildRank_Add(DBConnectionManager* manager, const GuildRank* record);
int GuildRank_Get(DBConnectionManager* manager, signed char server_id, int guild_id, GuildRank* record);
int GuildRank_Update(DBConnectionManager* manager, const GuildRank* record);
int GuildRank_Delete(DBConnectionManager* manager, signed char server_id, int guild_id);
int GuildRank_Exists(DBConnectionManager* manager, signed char server_id, int guild_id);
int GuildRank_GetAll(DBConnectionManager* manager, GuildRank* records, int max_count, int* actual_count);

#endif /* GUILD_RANK_ORM_H */