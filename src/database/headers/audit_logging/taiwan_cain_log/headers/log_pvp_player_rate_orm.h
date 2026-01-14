#ifndef LOG_PVP_PLAYER_RATE_ORM_H
#define LOG_PVP_PLAYER_RATE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    char occ_date[11];
    int single_one_to_one;
    int single_one_to_one_other;
    int team_one_to_one;
    int team_one_to_one_other;
    int relay_one_to_one;
    int relay_one_to_one_other;
} LogPvpPlayerRate;

/* CRUD Operations */
int LogPvpPlayerRate_Add(DBConnectionManager* manager, const LogPvpPlayerRate* record);
int LogPvpPlayerRate_Get(DBConnectionManager* manager, int channel_no, const char* occ_date, LogPvpPlayerRate* record);
int LogPvpPlayerRate_Update(DBConnectionManager* manager, const LogPvpPlayerRate* record);
int LogPvpPlayerRate_Delete(DBConnectionManager* manager, int channel_no, const char* occ_date);
int LogPvpPlayerRate_Exists(DBConnectionManager* manager, int channel_no, const char* occ_date);
int LogPvpPlayerRate_GetAll(DBConnectionManager* manager, LogPvpPlayerRate* records, int max_count, int* actual_count);

#endif /* LOG_PVP_PLAYER_RATE_ORM_H */