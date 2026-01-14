#ifndef GUILD_HALLOFFAME_HTML_ORM_H
#define GUILD_HALLOFFAME_HTML_ORM_H

#include "db_connection_manager.h"

#define MAX_HTML_SIZE 65535  /* TEXT field max size */

typedef struct {
    int fame_id;                  /* Fame ID (primary key) */
    char title[101];              /* Title (VARCHAR(100) -> 101) */
    char* html;                   /* HTML content (TEXT, dynamically allocated) */
    unsigned int html_length;     /* Actual length of HTML content */
} GuildHalloffameHtml;

/* Memory management */
GuildHalloffameHtml* GuildHalloffameHtml_Create(void);
void GuildHalloffameHtml_Free(GuildHalloffameHtml* hall);

/* CRUD Operations */
int GuildHalloffameHtml_Add(DBConnectionManager* manager, const GuildHalloffameHtml* hall);
int GuildHalloffameHtml_Get(DBConnectionManager* manager, int fame_id, GuildHalloffameHtml* hall);
int GuildHalloffameHtml_Update(DBConnectionManager* manager, const GuildHalloffameHtml* hall);
int GuildHalloffameHtml_Delete(DBConnectionManager* manager, int fame_id);
int GuildHalloffameHtml_Exists(DBConnectionManager* manager, int fame_id);

/* Business Operations */
int GuildHalloffameHtml_GetAll(DBConnectionManager* manager, GuildHalloffameHtml** halls,
                                int max_count, int* actual_count);
int GuildHalloffameHtml_GetByTitle(DBConnectionManager* manager, const char* title,
                                   GuildHalloffameHtml* hall);
int GuildHalloffameHtml_UpdateHtml(DBConnectionManager* manager, int fame_id,
                                   const char* new_html);

/* Utility Functions */
void GuildHalloffameHtml_PrintInfo(const GuildHalloffameHtml* hall);

#endif /* GUILD_HALLOFFAME_HTML_ORM_H */
