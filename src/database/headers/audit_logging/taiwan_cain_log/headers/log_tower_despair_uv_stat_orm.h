#ifndef LOG_TOWER_DESPAIR_UV_STAT_ORM_H
#define LOG_TOWER_DESPAIR_UV_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char server_id;
    int uv;
} LogTowerDespairUvStat;

/* CRUD Operations */
int LogTowerDespairUvStat_Add(DBConnectionManager* manager, const LogTowerDespairUvStat* record);
int LogTowerDespairUvStat_Get(DBConnectionManager* manager, const char* occ_date, signed char server_id, LogTowerDespairUvStat* record);
int LogTowerDespairUvStat_Update(DBConnectionManager* manager, const LogTowerDespairUvStat* record);
int LogTowerDespairUvStat_Delete(DBConnectionManager* manager, const char* occ_date, signed char server_id);
int LogTowerDespairUvStat_Exists(DBConnectionManager* manager, const char* occ_date, signed char server_id);
int LogTowerDespairUvStat_GetAll(DBConnectionManager* manager, LogTowerDespairUvStat* records, int max_count, int* actual_count);

#endif /* LOG_TOWER_DESPAIR_UV_STAT_ORM_H */