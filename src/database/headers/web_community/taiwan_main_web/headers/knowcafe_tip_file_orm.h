#ifndef KNOWCAFE_TIP_FILE_ORM_H
#define KNOWCAFE_TIP_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowcafeTipFile;

/* CRUD Operations */
int KnowcafeTipFile_Add(DBConnectionManager* manager, const KnowcafeTipFile* record);
int KnowcafeTipFile_Get(DBConnectionManager* manager, int no, KnowcafeTipFile* record);
int KnowcafeTipFile_Update(DBConnectionManager* manager, const KnowcafeTipFile* record);
int KnowcafeTipFile_Delete(DBConnectionManager* manager, int no);
int KnowcafeTipFile_Exists(DBConnectionManager* manager, int no);
int KnowcafeTipFile_GetAll(DBConnectionManager* manager, KnowcafeTipFile* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_TIP_FILE_ORM_H */