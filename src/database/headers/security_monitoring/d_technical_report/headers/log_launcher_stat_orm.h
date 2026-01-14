#ifndef LOG_LAUNCHER_STAT_ORM_H
#define LOG_LAUNCHER_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int execute;
    int cancel;
    int success;
    int first_success;
    double p2p;
    long long all_time;
    int p2p_count;
    int all_time_count;
} LogLauncherStat;

/* CRUD Operations */
int LogLauncherStat_Add(DBConnectionManager* manager, const LogLauncherStat* record);
int LogLauncherStat_Get(DBConnectionManager* manager, const char* occ_date, LogLauncherStat* record);
int LogLauncherStat_Update(DBConnectionManager* manager, const LogLauncherStat* record);
int LogLauncherStat_Delete(DBConnectionManager* manager, const char* occ_date);
int LogLauncherStat_Exists(DBConnectionManager* manager, const char* occ_date);
int LogLauncherStat_GetAll(DBConnectionManager* manager, LogLauncherStat* records, int max_count, int* actual_count);

#endif /* LOG_LAUNCHER_STAT_ORM_H */