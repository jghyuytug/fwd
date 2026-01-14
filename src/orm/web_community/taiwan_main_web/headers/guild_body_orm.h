#ifndef GUILD_BODY_ORM_H
#define GUILD_BODY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    char body[1024];
} GuildBody;

/* CRUD Operations */
int GuildBody_Add(DBConnectionManager* manager, const GuildBody* record);
int GuildBody_Get(DBConnectionManager* manager, int gno, GuildBody* record);
int GuildBody_Update(DBConnectionManager* manager, const GuildBody* record);
int GuildBody_Delete(DBConnectionManager* manager, int gno);
int GuildBody_Exists(DBConnectionManager* manager, int gno);
int GuildBody_GetAll(DBConnectionManager* manager, GuildBody* records, int max_count, int* actual_count);

#endif /* GUILD_BODY_ORM_H */