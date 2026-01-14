#ifndef GUILD_HALLOFFAME_ORM_H
#define GUILD_HALLOFFAME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int fame_id;
    unsigned char server_id;
    int guild_id;
    char guild_name[41];      /* VARCHAR(40) + 1 */
    char file_url[129];       /* VARCHAR(128) + 1 */
    unsigned char open_flag;
    unsigned char main_flag;
} GuildHalloffame;

/* CRUD operations - composite primary key (fame_id, server_id) */
int GuildHalloffame_Add(DBConnectionManager* manager, const GuildHalloffame* fame);
int GuildHalloffame_Get(DBConnectionManager* manager, int fame_id, unsigned char server_id,
                        GuildHalloffame* fame);
int GuildHalloffame_Update(DBConnectionManager* manager, const GuildHalloffame* fame);
int GuildHalloffame_Delete(DBConnectionManager* manager, int fame_id, unsigned char server_id);
int GuildHalloffame_Exists(DBConnectionManager* manager, int fame_id, unsigned char server_id);

/* Business operations */
int GuildHalloffame_GetByGuild(DBConnectionManager* manager, int guild_id,
                               GuildHalloffame* fames, int max_count, int* actual_count);
int GuildHalloffame_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                GuildHalloffame* fames, int max_count, int* actual_count);
int GuildHalloffame_GetByFlags(DBConnectionManager* manager, unsigned char open_flag,
                               unsigned char main_flag, GuildHalloffame* fames,
                               int max_count, int* actual_count);
int GuildHalloffame_DeleteByGuild(DBConnectionManager* manager, int guild_id);

/* Utility */
void GuildHalloffame_PrintInfo(const GuildHalloffame* fame);

#endif
