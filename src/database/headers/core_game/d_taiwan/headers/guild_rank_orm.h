#ifndef GUILD_RANK_ORM_H
#define GUILD_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_id;        /* Server ID (composite primary key) */
    int guild_id;                   /* Guild ID (composite primary key) */
    char guild_name[41];            /* Guild name (VARCHAR(40) -> 41) */
    unsigned short guild_Rank;      /* Guild rank */
    unsigned int guild_point;       /* Guild points */
    unsigned int guild_acc_point;   /* Accumulated guild points */
    unsigned int guild_visit;       /* Guild visits */
    unsigned int guild_acc_visit;   /* Accumulated guild visits */
    unsigned short guild_member;    /* Guild member count */
    unsigned short guild_acc_member;/* Accumulated guild members */
    unsigned short guild_avg_lev;   /* Average guild level */
} GuildRank;

/* CRUD Operations */
int GuildRank_Add(DBConnectionManager* manager, const GuildRank* rank);
int GuildRank_Get(DBConnectionManager* manager, unsigned char server_id,
                  int guild_id, GuildRank* rank);
int GuildRank_Update(DBConnectionManager* manager, const GuildRank* rank);
int GuildRank_Delete(DBConnectionManager* manager, unsigned char server_id, int guild_id);
int GuildRank_Exists(DBConnectionManager* manager, unsigned char server_id, int guild_id);

/* Business Operations */
int GuildRank_GetAll(DBConnectionManager* manager, GuildRank* ranks,
                     int max_count, int* actual_count);
int GuildRank_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                          GuildRank* ranks, int max_count, int* actual_count);
int GuildRank_GetTopRanks(DBConnectionManager* manager, unsigned char server_id,
                          int limit, GuildRank* ranks, int max_count, int* actual_count);
int GuildRank_GetByPointRange(DBConnectionManager* manager, unsigned char server_id,
                              unsigned int min_point, unsigned int max_point,
                              GuildRank* ranks, int max_count, int* actual_count);

/* Utility Functions */
void GuildRank_PrintInfo(const GuildRank* rank);

#endif /* GUILD_RANK_ORM_H */
