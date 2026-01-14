#ifndef GUILD_STAT_MONTH_ORM_H
#define GUILD_STAT_MONTH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char lev;
    signed char server_id;
    int avg_guild_point;
    int avg_guild_point_acc;
} GuildStatMonth;

/* CRUD Operations */
int GuildStatMonth_Add(DBConnectionManager* manager, const GuildStatMonth* record);
int GuildStatMonth_Get(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id, GuildStatMonth* record);
int GuildStatMonth_Update(DBConnectionManager* manager, const GuildStatMonth* record);
int GuildStatMonth_Delete(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id);
int GuildStatMonth_Exists(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id);
int GuildStatMonth_GetAll(DBConnectionManager* manager, GuildStatMonth* records, int max_count, int* actual_count);

#endif /* GUILD_STAT_MONTH_ORM_H */