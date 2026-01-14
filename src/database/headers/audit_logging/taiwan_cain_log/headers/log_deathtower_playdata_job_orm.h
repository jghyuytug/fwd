#ifndef LOG_DEATHTOWER_PLAYDATA_JOB_ORM_H
#define LOG_DEATHTOWER_PLAYDATA_JOB_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char type;
    signed char charac_job;
    signed char charac_grow;
    signed char level;
    int playcount;
    int avg_clear_count;
} LogDeathtowerPlaydataJob;

/* CRUD Operations */
int LogDeathtowerPlaydataJob_Add(DBConnectionManager* manager, const LogDeathtowerPlaydataJob* record);
int LogDeathtowerPlaydataJob_Get(DBConnectionManager* manager, const char* occ_time, signed char type, signed char charac_job, signed char charac_grow, signed char level, LogDeathtowerPlaydataJob* record);
int LogDeathtowerPlaydataJob_Update(DBConnectionManager* manager, const LogDeathtowerPlaydataJob* record);
int LogDeathtowerPlaydataJob_Delete(DBConnectionManager* manager, const char* occ_time, signed char type, signed char charac_job, signed char charac_grow, signed char level);
int LogDeathtowerPlaydataJob_Exists(DBConnectionManager* manager, const char* occ_time, signed char type, signed char charac_job, signed char charac_grow, signed char level);
int LogDeathtowerPlaydataJob_GetAll(DBConnectionManager* manager, LogDeathtowerPlaydataJob* records, int max_count, int* actual_count);

#endif /* LOG_DEATHTOWER_PLAYDATA_JOB_ORM_H */