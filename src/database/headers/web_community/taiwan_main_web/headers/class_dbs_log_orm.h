#ifndef CLASS_DBS_LOG_ORM_H
#define CLASS_DBS_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int log_id;
    char memo[8192];
    char reg_date[20];
} ClassDbsLog;

/* CRUD Operations */
int ClassDbsLog_Add(DBConnectionManager* manager, const ClassDbsLog* record);
int ClassDbsLog_Get(DBConnectionManager* manager, int log_id, ClassDbsLog* record);
int ClassDbsLog_Update(DBConnectionManager* manager, const ClassDbsLog* record);
int ClassDbsLog_Delete(DBConnectionManager* manager, int log_id);
int ClassDbsLog_Exists(DBConnectionManager* manager, int log_id);
int ClassDbsLog_GetAll(DBConnectionManager* manager, ClassDbsLog* records, int max_count, int* actual_count);

#endif /* CLASS_DBS_LOG_ORM_H */