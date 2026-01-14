#ifndef USER_TING_TIMECHECK_ORM_H
#define USER_TING_TIMECHECK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int minute;
    int cnt;
} UserTingTimecheck;

/* CRUD Operations */
int UserTingTimecheck_Add(DBConnectionManager* manager, const UserTingTimecheck* record);
int UserTingTimecheck_Get(DBConnectionManager* manager, const char* occ_time, int minute, UserTingTimecheck* record);
int UserTingTimecheck_Update(DBConnectionManager* manager, const UserTingTimecheck* record);
int UserTingTimecheck_Delete(DBConnectionManager* manager, const char* occ_time, int minute);
int UserTingTimecheck_Exists(DBConnectionManager* manager, const char* occ_time, int minute);
int UserTingTimecheck_GetAll(DBConnectionManager* manager, UserTingTimecheck* records, int max_count, int* actual_count);

#endif /* USER_TING_TIMECHECK_ORM_H */