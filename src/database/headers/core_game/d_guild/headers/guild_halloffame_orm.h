#ifndef GUILD_HALLOFFAME_ORM_H
#define GUILD_HALLOFFAME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int fame_id;
    signed char server_id;
    int guild_id;
    char guild_name[41];
    char file_url[129];
    signed char open_flag;
    signed char main_flag;
} GuildHalloffame;

/* CRUD Operations */
int GuildHalloffame_Add(DBConnectionManager* manager, const GuildHalloffame* record);
int GuildHalloffame_Get(DBConnectionManager* manager, int fame_id, signed char server_id, GuildHalloffame* record);
int GuildHalloffame_Update(DBConnectionManager* manager, const GuildHalloffame* record);
int GuildHalloffame_Delete(DBConnectionManager* manager, int fame_id, signed char server_id);
int GuildHalloffame_Exists(DBConnectionManager* manager, int fame_id, signed char server_id);
int GuildHalloffame_GetAll(DBConnectionManager* manager, GuildHalloffame* records, int max_count, int* actual_count);

#endif /* GUILD_HALLOFFAME_ORM_H */