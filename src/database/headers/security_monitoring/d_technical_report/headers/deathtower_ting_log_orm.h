#ifndef DEATHTOWER_TING_LOG_ORM_H
#define DEATHTOWER_TING_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char level;
    int ting_cnt;
} DeathtowerTingLog;

/* CRUD Operations */
int DeathtowerTingLog_Add(DBConnectionManager* manager, const DeathtowerTingLog* record);
int DeathtowerTingLog_Get(DBConnectionManager* manager, const char* occ_time, signed char level, DeathtowerTingLog* record);
int DeathtowerTingLog_Update(DBConnectionManager* manager, const DeathtowerTingLog* record);
int DeathtowerTingLog_Delete(DBConnectionManager* manager, const char* occ_time, signed char level);
int DeathtowerTingLog_Exists(DBConnectionManager* manager, const char* occ_time, signed char level);
int DeathtowerTingLog_GetAll(DBConnectionManager* manager, DeathtowerTingLog* records, int max_count, int* actual_count);

#endif /* DEATHTOWER_TING_LOG_ORM_H */