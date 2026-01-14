#ifndef POWER_WAR_H
#define POWER_WAR_H

#include "db_connection_manager.h"

typedef struct {
    char server_id;
    unsigned int a_side_point;
    unsigned int b_side_point;
    char winner_side;
    char occ_time[20];
} PowerWar;

// CRUD Operations
int PowerWar_Add(DBConnectionManager* manager, const PowerWar* record);
int PowerWar_Get(DBConnectionManager* manager, char server_id, PowerWar* out_record);
int PowerWar_Update(DBConnectionManager* manager, const PowerWar* record);
int PowerWar_Delete(DBConnectionManager* manager, char server_id);
int PowerWar_Exists(DBConnectionManager* manager, char server_id);

// Business Logic
int PowerWar_GetWinnerServers(DBConnectionManager* manager, PowerWar** out_records, int* out_count);
int PowerWar_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, PowerWar** out_records, int* out_count);
int PowerWar_GetLeaderboard(DBConnectionManager* manager, int limit, PowerWar** out_records, int* out_count);

#endif // POWER_WAR_H
