#ifndef GUILD_STAT_ORM_H
#define GUILD_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];          /* DATE field YYYY-MM-DD */
    unsigned char lev;          /* tinyint(3) unsigned */
    unsigned char server_id;    /* tinyint(4) */
    int create_no;
    int acc_create_no;
    int member_no;
    int acc_member_no;
    float avg_lev;
    float avg_master_lev;
    int expire_no;
    int new_account_no;
    int new_member_no;
    int acc_account_no;
} GuildStat;

/* CRUD Operations */
int GuildStat_Add(DBConnectionManager* manager, const GuildStat* stat);
int GuildStat_Get(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                   unsigned char server_id, GuildStat* stat);
int GuildStat_Update(DBConnectionManager* manager, const GuildStat* stat);
int GuildStat_Delete(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                      unsigned char server_id);
int GuildStat_Exists(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                      unsigned char server_id);

/* Business Operations */
int GuildStat_GetByDate(DBConnectionManager* manager, const char* occ_date,
                        GuildStat* stats, int max_count, int* actual_count);
int GuildStat_GetByDateAndLevel(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                                  GuildStat* stats, int max_count, int* actual_count);
int GuildStat_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                           GuildStat* stats, int max_count, int* actual_count);
int GuildStat_DeleteByDate(DBConnectionManager* manager, const char* occ_date);

/* Utility Functions */
void GuildStat_PrintInfo(const GuildStat* stat);

#endif /* GUILD_STAT_ORM_H */
