#ifndef GUILD_NOTICE_H
#define GUILD_NOTICE_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int guild_id;
    char notice[201];
    int acc_date;
} GuildNotice;

// CRUD Operations
int GuildNotice_Add(DBConnectionManager* manager, const GuildNotice* notice);
int GuildNotice_Get(DBConnectionManager* manager, unsigned int guild_id, GuildNotice* notice);
int GuildNotice_Update(DBConnectionManager* manager, const GuildNotice* notice);
int GuildNotice_Delete(DBConnectionManager* manager, unsigned int guild_id);
int GuildNotice_Exists(DBConnectionManager* manager, unsigned int guild_id);

// Business Operations
int GuildNotice_GetAll(DBConnectionManager* manager, GuildNotice** notices, int* count);
int GuildNotice_GetRecentNotices(DBConnectionManager* manager, int limit, GuildNotice** notices, int* count);
int GuildNotice_UpdateNotice(DBConnectionManager* manager, unsigned int guild_id, const char* notice, int acc_date);

#endif // GUILD_NOTICE_H
