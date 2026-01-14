#ifndef GUILD_VISIT_ORM_H
#define GUILD_VISIT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;               /* Guild ID (primary key) */
    unsigned char server_id;    /* Server ID */
    int total_visit;            /* Total visit count */
    int today_visit;            /* Today's visit count */
} GuildVisit;

/* CRUD Operations */
int GuildVisit_Add(DBConnectionManager* manager, const GuildVisit* visit);
int GuildVisit_Get(DBConnectionManager* manager, int guild_id, GuildVisit* visit);
int GuildVisit_Update(DBConnectionManager* manager, const GuildVisit* visit);
int GuildVisit_Delete(DBConnectionManager* manager, int guild_id);
int GuildVisit_Exists(DBConnectionManager* manager, int guild_id);

/* Business Operations */
int GuildVisit_IncrementVisit(DBConnectionManager* manager, int guild_id, int increment_total, int increment_today);
int GuildVisit_ResetTodayVisits(DBConnectionManager* manager);
int GuildVisit_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                            GuildVisit* visits, int max_count, int* actual_count);
int GuildVisit_GetTopVisited(DBConnectionManager* manager, int limit,
                              GuildVisit* visits, int* actual_count);

/* Utility Functions */
void GuildVisit_PrintInfo(const GuildVisit* visit);

#endif
