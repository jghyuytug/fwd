#ifndef LOG_FATIGUE_DAILY_ORM_H
#define LOG_FATIGUE_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int fatigue;
} LogFatigueDaily;

/* CRUD Operations */
int LogFatigueDaily_Add(DBConnectionManager* manager, const LogFatigueDaily* record);
int LogFatigueDaily_Get(DBConnectionManager* manager, const char* occ_date, LogFatigueDaily* record);
int LogFatigueDaily_Update(DBConnectionManager* manager, const LogFatigueDaily* record);
int LogFatigueDaily_Delete(DBConnectionManager* manager, const char* occ_date);
int LogFatigueDaily_Exists(DBConnectionManager* manager, const char* occ_date);
int LogFatigueDaily_GetAll(DBConnectionManager* manager, LogFatigueDaily* records, int max_count, int* actual_count);

#endif /* LOG_FATIGUE_DAILY_ORM_H */