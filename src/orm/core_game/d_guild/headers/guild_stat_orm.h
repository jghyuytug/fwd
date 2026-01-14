#ifndef GUILD_STAT_ORM_H
#define GUILD_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char lev;
    signed char server_id;
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
int GuildStat_Add(DBConnectionManager* manager, const GuildStat* record);
int GuildStat_Get(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id, GuildStat* record);
int GuildStat_Update(DBConnectionManager* manager, const GuildStat* record);
int GuildStat_Delete(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id);
int GuildStat_Exists(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id);
int GuildStat_GetAll(DBConnectionManager* manager, GuildStat* records, int max_count, int* actual_count);

#endif /* GUILD_STAT_ORM_H */