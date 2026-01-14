#ifndef GUILD_JOIN_LIST_ORM_H
#define GUILD_JOIN_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    int charac_no;
    signed char server_group;
    int m_id;
    char born_year[3];
    char memo[26];
    char occ_time[20];
} GuildJoinList;

/* CRUD Operations */
int GuildJoinList_Add(DBConnectionManager* manager, const GuildJoinList* record);
int GuildJoinList_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildJoinList* record);
int GuildJoinList_Update(DBConnectionManager* manager, const GuildJoinList* record);
int GuildJoinList_Delete(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildJoinList_Exists(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildJoinList_GetAll(DBConnectionManager* manager, GuildJoinList* records, int max_count, int* actual_count);

#endif /* GUILD_JOIN_LIST_ORM_H */