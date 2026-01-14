#ifndef GUILD_STAT_UV_ORM_H
#define GUILD_STAT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int pv;
    int new_bbs;
    int total_read_bbs;
    int member_uv;
    int member_uv_week;
    int master_uv;
    int master_uv_week;
} GuildStatUv;

/* CRUD Operations */
int GuildStatUv_Add(DBConnectionManager* manager, const GuildStatUv* record);
int GuildStatUv_Get(DBConnectionManager* manager, const char* occ_date, GuildStatUv* record);
int GuildStatUv_Update(DBConnectionManager* manager, const GuildStatUv* record);
int GuildStatUv_Delete(DBConnectionManager* manager, const char* occ_date);
int GuildStatUv_Exists(DBConnectionManager* manager, const char* occ_date);
int GuildStatUv_GetAll(DBConnectionManager* manager, GuildStatUv* records, int max_count, int* actual_count);

#endif /* GUILD_STAT_UV_ORM_H */