#ifndef JOB_LIST_ORM_H
#define JOB_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char job_code;
    char job_name[11];
} JobList;

/* CRUD Operations */
int JobList_Add(DBConnectionManager* manager, const JobList* record);
int JobList_Get(DBConnectionManager* manager, signed char job_code, JobList* record);
int JobList_Update(DBConnectionManager* manager, const JobList* record);
int JobList_Delete(DBConnectionManager* manager, signed char job_code);
int JobList_Exists(DBConnectionManager* manager, signed char job_code);
int JobList_GetAll(DBConnectionManager* manager, JobList* records, int max_count, int* actual_count);

#endif /* JOB_LIST_ORM_H */