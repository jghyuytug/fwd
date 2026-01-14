#ifndef GUILD_EVENT_ORM_H
#define GUILD_EVENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;                 /* Guild number (primary key) */
    char stt_date[11];       /* Start date (YYYY-MM-DD) */
    char end_date[11];       /* End date (YYYY-MM-DD) */
    char ann_date[11];       /* Announce date (YYYY-MM-DD) */
    char page_url[101];      /* Page URL (VARCHAR(100) -> 101) */
} GuildEvent;

/* CRUD Operations */
int GuildEvent_Add(DBConnectionManager* manager, const GuildEvent* event);
int GuildEvent_Get(DBConnectionManager* manager, int gno, GuildEvent* event);
int GuildEvent_Update(DBConnectionManager* manager, const GuildEvent* event);
int GuildEvent_Delete(DBConnectionManager* manager, int gno);
int GuildEvent_Exists(DBConnectionManager* manager, int gno);

/* Business Operations */
int GuildEvent_GetAll(DBConnectionManager* manager, GuildEvent* events,
                      int max_count, int* actual_count);
int GuildEvent_GetActive(DBConnectionManager* manager, const char* current_date,
                         GuildEvent* events, int max_count, int* actual_count);
int GuildEvent_GetByDateRange(DBConnectionManager* manager,
                               const char* start_date, const char* end_date,
                               GuildEvent* events, int max_count, int* actual_count);

/* Utility Functions */
void GuildEvent_PrintInfo(const GuildEvent* event);

#endif /* GUILD_EVENT_ORM_H */
