#ifndef LOADING_TIME_ORM_H
#define LOADING_TIME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_id;
    signed char type;
    short load_sec;
} LoadingTime;

/* CRUD Operations */
int LoadingTime_Add(DBConnectionManager* manager, const LoadingTime* record);
int LoadingTime_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char type, LoadingTime* record);
int LoadingTime_Update(DBConnectionManager* manager, const LoadingTime* record);
int LoadingTime_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char type);
int LoadingTime_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char type);
int LoadingTime_GetAll(DBConnectionManager* manager, LoadingTime* records, int max_count, int* actual_count);

#endif /* LOADING_TIME_ORM_H */