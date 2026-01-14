#ifndef GUILD_SEARCH_ORM_H
#define GUILD_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    signed char server_id;
    char guild_name[41];
    char master_name[21];
    char create_time[20];
    int lev;
    int member_count;
    int guild_point_acc;
    int guild_exp;
    char guild_url[41];
} GuildSearch;

/* CRUD Operations */
int GuildSearch_Add(DBConnectionManager* manager, const GuildSearch* record);
int GuildSearch_Get(DBConnectionManager* manager, int guild_id, GuildSearch* record);
int GuildSearch_Update(DBConnectionManager* manager, const GuildSearch* record);
int GuildSearch_Delete(DBConnectionManager* manager, int guild_id);
int GuildSearch_Exists(DBConnectionManager* manager, int guild_id);
int GuildSearch_GetAll(DBConnectionManager* manager, GuildSearch* records, int max_count, int* actual_count);

#endif /* GUILD_SEARCH_ORM_H */