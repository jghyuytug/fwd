#ifndef GUILD_INTRODUCE_ORM_H
#define GUILD_INTRODUCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;               /* Guild ID (primary key) */
    unsigned char server_id;    /* Server ID */
    char introduce[201];        /* Guild introduction (varchar 200 + 1) */
} GuildIntroduce;

/* CRUD Operations */
int GuildIntroduce_Add(DBConnectionManager* manager, const GuildIntroduce* intro);
int GuildIntroduce_Get(DBConnectionManager* manager, int guild_id, GuildIntroduce* intro);
int GuildIntroduce_Update(DBConnectionManager* manager, const GuildIntroduce* intro);
int GuildIntroduce_Delete(DBConnectionManager* manager, int guild_id);
int GuildIntroduce_Exists(DBConnectionManager* manager, int guild_id);

/* Business Operations */
int GuildIntroduce_UpdateIntroduce(DBConnectionManager* manager, int guild_id,
                                    const char* new_introduce);
int GuildIntroduce_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                GuildIntroduce* intros, int max_count, int* actual_count);

/* Utility Functions */
void GuildIntroduce_PrintInfo(const GuildIntroduce* intro);

#endif
