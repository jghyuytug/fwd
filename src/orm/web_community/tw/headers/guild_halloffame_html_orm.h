#ifndef GUILD_HALLOFFAME_HTML_ORM_H
#define GUILD_HALLOFFAME_HTML_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int fame_id;
    char title[101];
    char html[1024];
} GuildHalloffameHtml;

/* CRUD Operations */
int GuildHalloffameHtml_Add(DBConnectionManager* manager, const GuildHalloffameHtml* record);
int GuildHalloffameHtml_Get(DBConnectionManager* manager, int fame_id, GuildHalloffameHtml* record);
int GuildHalloffameHtml_Update(DBConnectionManager* manager, const GuildHalloffameHtml* record);
int GuildHalloffameHtml_Delete(DBConnectionManager* manager, int fame_id);
int GuildHalloffameHtml_Exists(DBConnectionManager* manager, int fame_id);
int GuildHalloffameHtml_GetAll(DBConnectionManager* manager, GuildHalloffameHtml* records, int max_count, int* actual_count);

#endif /* GUILD_HALLOFFAME_HTML_ORM_H */