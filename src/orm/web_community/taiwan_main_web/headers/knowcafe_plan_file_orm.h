#ifndef KNOWCAFE_PLAN_FILE_ORM_H
#define KNOWCAFE_PLAN_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowcafePlanFile;

/* CRUD Operations */
int KnowcafePlanFile_Add(DBConnectionManager* manager, const KnowcafePlanFile* record);
int KnowcafePlanFile_Get(DBConnectionManager* manager, int no, KnowcafePlanFile* record);
int KnowcafePlanFile_Update(DBConnectionManager* manager, const KnowcafePlanFile* record);
int KnowcafePlanFile_Delete(DBConnectionManager* manager, int no);
int KnowcafePlanFile_Exists(DBConnectionManager* manager, int no);
int KnowcafePlanFile_GetAll(DBConnectionManager* manager, KnowcafePlanFile* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_PLAN_FILE_ORM_H */