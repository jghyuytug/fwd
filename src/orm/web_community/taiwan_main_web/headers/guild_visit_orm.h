#ifndef GUILD_VISIT_ORM_H
#define GUILD_VISIT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    signed char server_id;
    int total_visit;
    int today_visit;
} GuildVisit;

/* CRUD Operations */
int GuildVisit_Add(DBConnectionManager* manager, const GuildVisit* record);
int GuildVisit_Get(DBConnectionManager* manager, int guild_id, GuildVisit* record);
int GuildVisit_Update(DBConnectionManager* manager, const GuildVisit* record);
int GuildVisit_Delete(DBConnectionManager* manager, int guild_id);
int GuildVisit_Exists(DBConnectionManager* manager, int guild_id);
int GuildVisit_GetAll(DBConnectionManager* manager, GuildVisit* records, int max_count, int* actual_count);

#endif /* GUILD_VISIT_ORM_H */