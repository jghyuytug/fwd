#ifndef KNOWCAFE_JOB_FILE_ORM_H
#define KNOWCAFE_JOB_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowcafeJobFile;

/* CRUD Operations */
int KnowcafeJobFile_Add(DBConnectionManager* manager, const KnowcafeJobFile* record);
int KnowcafeJobFile_Get(DBConnectionManager* manager, int no, KnowcafeJobFile* record);
int KnowcafeJobFile_Update(DBConnectionManager* manager, const KnowcafeJobFile* record);
int KnowcafeJobFile_Delete(DBConnectionManager* manager, int no);
int KnowcafeJobFile_Exists(DBConnectionManager* manager, int no);
int KnowcafeJobFile_GetAll(DBConnectionManager* manager, KnowcafeJobFile* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_JOB_FILE_ORM_H */