#ifndef GUILD_NOTICE_ORM_H
#define GUILD_NOTICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    char notice[201];
    int acc_date;
} GuildNotice;

/* CRUD Operations */
int GuildNotice_Add(DBConnectionManager* manager, const GuildNotice* record);
int GuildNotice_Get(DBConnectionManager* manager, int guild_id, GuildNotice* record);
int GuildNotice_Update(DBConnectionManager* manager, const GuildNotice* record);
int GuildNotice_Delete(DBConnectionManager* manager, int guild_id);
int GuildNotice_Exists(DBConnectionManager* manager, int guild_id);
int GuildNotice_GetAll(DBConnectionManager* manager, GuildNotice* records, int max_count, int* actual_count);

#endif /* GUILD_NOTICE_ORM_H */