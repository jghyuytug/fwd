#ifndef POWER_WAR_GUILD_RANK_ORM_H
#define POWER_WAR_GUILD_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    short rank;
    int guild_id;
    int power_war_point;
    signed char power_side;
} PowerWarGuildRank;

/* CRUD Operations */
int PowerWarGuildRank_Add(DBConnectionManager* manager, const PowerWarGuildRank* record);
int PowerWarGuildRank_Get(DBConnectionManager* manager, int guild_id, PowerWarGuildRank* record);
int PowerWarGuildRank_Update(DBConnectionManager* manager, const PowerWarGuildRank* record);
int PowerWarGuildRank_Delete(DBConnectionManager* manager, int guild_id);
int PowerWarGuildRank_Exists(DBConnectionManager* manager, int guild_id);
int PowerWarGuildRank_GetAll(DBConnectionManager* manager, PowerWarGuildRank* records, int max_count, int* actual_count);

#endif /* POWER_WAR_GUILD_RANK_ORM_H */