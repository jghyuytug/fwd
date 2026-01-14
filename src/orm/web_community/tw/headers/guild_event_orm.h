#ifndef GUILD_EVENT_ORM_H
#define GUILD_EVENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    char stt_date[11];
    char end_date[11];
    char ann_date[11];
    char page_url[101];
} GuildEvent;

/* CRUD Operations */
int GuildEvent_Add(DBConnectionManager* manager, const GuildEvent* record);
int GuildEvent_Get(DBConnectionManager* manager, int gno, GuildEvent* record);
int GuildEvent_Update(DBConnectionManager* manager, const GuildEvent* record);
int GuildEvent_Delete(DBConnectionManager* manager, int gno);
int GuildEvent_Exists(DBConnectionManager* manager, int gno);
int GuildEvent_GetAll(DBConnectionManager* manager, GuildEvent* records, int max_count, int* actual_count);

#endif /* GUILD_EVENT_ORM_H */