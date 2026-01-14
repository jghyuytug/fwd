#ifndef GUILD_INTRODUCE_ORM_H
#define GUILD_INTRODUCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    signed char server_id;
    char introduce[201];
} GuildIntroduce;

/* CRUD Operations */
int GuildIntroduce_Add(DBConnectionManager* manager, const GuildIntroduce* record);
int GuildIntroduce_Get(DBConnectionManager* manager, int guild_id, GuildIntroduce* record);
int GuildIntroduce_Update(DBConnectionManager* manager, const GuildIntroduce* record);
int GuildIntroduce_Delete(DBConnectionManager* manager, int guild_id);
int GuildIntroduce_Exists(DBConnectionManager* manager, int guild_id);
int GuildIntroduce_GetAll(DBConnectionManager* manager, GuildIntroduce* records, int max_count, int* actual_count);

#endif /* GUILD_INTRODUCE_ORM_H */