#ifndef POWER_WAR_GUILD_RANK_H
#define POWER_WAR_GUILD_RANK_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_id;
    unsigned short rank;
    unsigned int guild_id;
    unsigned int power_war_point;
    unsigned char power_side;
} PowerWarGuildRank;

// CRUD Operations
int PowerWarGuildRank_Add(DBConnectionManager* manager, const PowerWarGuildRank* record);
int PowerWarGuildRank_Get(DBConnectionManager* manager, unsigned int guild_id, PowerWarGuildRank* out_record);
int PowerWarGuildRank_Update(DBConnectionManager* manager, const PowerWarGuildRank* record);
int PowerWarGuildRank_Delete(DBConnectionManager* manager, unsigned int guild_id);
int PowerWarGuildRank_Exists(DBConnectionManager* manager, unsigned int guild_id);

// Business Logic
int PowerWarGuildRank_GetByServer(DBConnectionManager* manager, unsigned char server_id, PowerWarGuildRank** out_records, int* out_count);
int PowerWarGuildRank_GetBySide(DBConnectionManager* manager, unsigned char power_side, PowerWarGuildRank** out_records, int* out_count);
int PowerWarGuildRank_GetTopRanks(DBConnectionManager* manager, unsigned char server_id, int limit, PowerWarGuildRank** out_records, int* out_count);

#endif // POWER_WAR_GUILD_RANK_H
