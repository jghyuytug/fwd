#ifndef DEATHTOWER_TING_LOG_DAILY_ORM_H
#define DEATHTOWER_TING_LOG_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char level;
    int ting_cnt;
} DeathtowerTingLogDaily;

/* CRUD Operations */
int DeathtowerTingLogDaily_Add(DBConnectionManager* manager, const DeathtowerTingLogDaily* record);
int DeathtowerTingLogDaily_Get(DBConnectionManager* manager, const char* occ_date, signed char level, DeathtowerTingLogDaily* record);
int DeathtowerTingLogDaily_Update(DBConnectionManager* manager, const DeathtowerTingLogDaily* record);
int DeathtowerTingLogDaily_Delete(DBConnectionManager* manager, const char* occ_date, signed char level);
int DeathtowerTingLogDaily_Exists(DBConnectionManager* manager, const char* occ_date, signed char level);
int DeathtowerTingLogDaily_GetAll(DBConnectionManager* manager, DeathtowerTingLogDaily* records, int max_count, int* actual_count);

#endif /* DEATHTOWER_TING_LOG_DAILY_ORM_H */