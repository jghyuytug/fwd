#ifndef GUILD_BODY_ORM_H
#define GUILD_BODY_ORM_H

#include "db_connection_manager.h"

#define GUILD_BODY_MAX_LEN 65535  /* TEXT field max length */

typedef struct {
    int gno;
    char* body;  /* Dynamically allocated for TEXT field */
} GuildBody;

/* CRUD Operations */
int GuildBody_Add(DBConnectionManager* manager, const GuildBody* guild);
int GuildBody_Get(DBConnectionManager* manager, int gno, GuildBody* guild);
int GuildBody_Update(DBConnectionManager* manager, const GuildBody* guild);
int GuildBody_Delete(DBConnectionManager* manager, int gno);
int GuildBody_Exists(DBConnectionManager* manager, int gno);

/* Utility Functions */
void GuildBody_PrintInfo(const GuildBody* guild);
void GuildBody_Free(GuildBody* guild);  /* Free dynamically allocated memory */
GuildBody* GuildBody_Allocate(void);    /* Allocate new GuildBody */

#endif /* GUILD_BODY_ORM_H */
