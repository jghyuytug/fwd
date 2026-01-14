#ifndef GUILD_JOIN_LIST_H
#define GUILD_JOIN_LIST_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int guild_id;
    unsigned int charac_no;
    char server_group;
    unsigned int m_id;
    char born_year[3];
    char memo[26];
    char occ_time[20];
} GuildJoinList;

// CRUD Operations (Composite primary key: guild_id, charac_no)
int GuildJoinList_Add(DBConnectionManager* manager, const GuildJoinList* join);
int GuildJoinList_Get(DBConnectionManager* manager, unsigned int guild_id, unsigned int charac_no, GuildJoinList* join);
int GuildJoinList_Update(DBConnectionManager* manager, const GuildJoinList* join);
int GuildJoinList_Delete(DBConnectionManager* manager, unsigned int guild_id, unsigned int charac_no);
int GuildJoinList_Exists(DBConnectionManager* manager, unsigned int guild_id, unsigned int charac_no);

// Business Operations
int GuildJoinList_GetByGuild(DBConnectionManager* manager, unsigned int guild_id, GuildJoinList** joins, int* count);
int GuildJoinList_GetByCharacter(DBConnectionManager* manager, unsigned int charac_no, GuildJoinList** joins, int* count);
int GuildJoinList_GetByServer(DBConnectionManager* manager, char server_group, GuildJoinList** joins, int* count);

#endif // GUILD_JOIN_LIST_H
