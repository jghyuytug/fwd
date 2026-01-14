#ifndef GUILD_STAT_MONTH_ORM_H
#define GUILD_STAT_MONTH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];          /* DATE field YYYY-MM-DD */
    unsigned char lev;          /* tinyint(3) unsigned */
    unsigned char server_id;    /* tinyint(4) */
    int avg_guild_point;
    int avg_guild_point_acc;
} GuildStatMonth;

/* CRUD Operations */
int GuildStatMonth_Add(DBConnectionManager* manager, const GuildStatMonth* stat);
int GuildStatMonth_Get(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                       unsigned char server_id, GuildStatMonth* stat);
int GuildStatMonth_Update(DBConnectionManager* manager, const GuildStatMonth* stat);
int GuildStatMonth_Delete(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                          unsigned char server_id);
int GuildStatMonth_Exists(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                          unsigned char server_id);

/* Business Operations */
int GuildStatMonth_GetByDate(DBConnectionManager* manager, const char* occ_date,
                             GuildStatMonth* stats, int max_count, int* actual_count);
int GuildStatMonth_GetByDateAndLevel(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                                      GuildStatMonth* stats, int max_count, int* actual_count);
int GuildStatMonth_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                GuildStatMonth* stats, int max_count, int* actual_count);
int GuildStatMonth_DeleteByDate(DBConnectionManager* manager, const char* occ_date);

/* Utility Functions */
void GuildStatMonth_PrintInfo(const GuildStatMonth* stat);

#endif /* GUILD_STAT_MONTH_ORM_H */
