#ifndef POWER_WAR_USER_RANK_H
#define POWER_WAR_USER_RANK_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_id;
    unsigned short rank;
    unsigned int charac_no;
    unsigned int power_war_point;
    unsigned char power_side;
} PowerWarUserRank;

// CRUD Operations - IMPORTANT: Composite primary key (charac_no, server_id)
int PowerWarUserRank_Add(DBConnectionManager* manager, const PowerWarUserRank* record);
int PowerWarUserRank_Get(DBConnectionManager* manager, unsigned int charac_no, unsigned char server_id, PowerWarUserRank* out_record);
int PowerWarUserRank_Update(DBConnectionManager* manager, const PowerWarUserRank* record);
int PowerWarUserRank_Delete(DBConnectionManager* manager, unsigned int charac_no, unsigned char server_id);
int PowerWarUserRank_Exists(DBConnectionManager* manager, unsigned int charac_no, unsigned char server_id);

// Business Logic
int PowerWarUserRank_GetByServer(DBConnectionManager* manager, unsigned char server_id, PowerWarUserRank** out_records, int* out_count);
int PowerWarUserRank_GetBySide(DBConnectionManager* manager, unsigned char power_side, PowerWarUserRank** out_records, int* out_count);
int PowerWarUserRank_GetTopRanks(DBConnectionManager* manager, unsigned char server_id, int limit, PowerWarUserRank** out_records, int* out_count);

#endif // POWER_WAR_USER_RANK_H
