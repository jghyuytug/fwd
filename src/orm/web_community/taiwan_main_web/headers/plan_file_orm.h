#ifndef PLAN_FILE_ORM_H
#define PLAN_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} PlanFile;

/* CRUD Operations */
int PlanFile_Add(DBConnectionManager* manager, const PlanFile* record);
int PlanFile_Get(DBConnectionManager* manager, int no, PlanFile* record);
int PlanFile_Update(DBConnectionManager* manager, const PlanFile* record);
int PlanFile_Delete(DBConnectionManager* manager, int no);
int PlanFile_Exists(DBConnectionManager* manager, int no);
int PlanFile_GetAll(DBConnectionManager* manager, PlanFile* records, int max_count, int* actual_count);

#endif /* PLAN_FILE_ORM_H */