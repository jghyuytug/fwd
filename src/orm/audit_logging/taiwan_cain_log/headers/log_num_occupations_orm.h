#ifndef LOG_NUM_OCCUPATIONS_ORM_H
#define LOG_NUM_OCCUPATIONS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int num_occupations_charscreen;
    int num_occupations_seriaroom;
    int num_login_per_min;
    int num_logout_per_min;
} LogNumOccupations;

/* CRUD Operations */
int LogNumOccupations_Add(DBConnectionManager* manager, const LogNumOccupations* record);
int LogNumOccupations_Get(DBConnectionManager* manager, const char* occ_time, LogNumOccupations* record);
int LogNumOccupations_Update(DBConnectionManager* manager, const LogNumOccupations* record);
int LogNumOccupations_Delete(DBConnectionManager* manager, const char* occ_time);
int LogNumOccupations_Exists(DBConnectionManager* manager, const char* occ_time);
int LogNumOccupations_GetAll(DBConnectionManager* manager, LogNumOccupations* records, int max_count, int* actual_count);

#endif /* LOG_NUM_OCCUPATIONS_ORM_H */