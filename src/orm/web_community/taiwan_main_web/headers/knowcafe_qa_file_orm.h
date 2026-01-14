#ifndef KNOWCAFE_QA_FILE_ORM_H
#define KNOWCAFE_QA_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowcafeQaFile;

/* CRUD Operations */
int KnowcafeQaFile_Add(DBConnectionManager* manager, const KnowcafeQaFile* record);
int KnowcafeQaFile_Get(DBConnectionManager* manager, int no, KnowcafeQaFile* record);
int KnowcafeQaFile_Update(DBConnectionManager* manager, const KnowcafeQaFile* record);
int KnowcafeQaFile_Delete(DBConnectionManager* manager, int no);
int KnowcafeQaFile_Exists(DBConnectionManager* manager, int no);
int KnowcafeQaFile_GetAll(DBConnectionManager* manager, KnowcafeQaFile* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_FILE_ORM_H */