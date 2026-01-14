#ifndef KNOWSTANDBY_FILE_ORM_H
#define KNOWSTANDBY_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowstandbyFile;

/* CRUD Operations */
int KnowstandbyFile_Add(DBConnectionManager* manager, const KnowstandbyFile* record);
int KnowstandbyFile_Get(DBConnectionManager* manager, int no, KnowstandbyFile* record);
int KnowstandbyFile_Update(DBConnectionManager* manager, const KnowstandbyFile* record);
int KnowstandbyFile_Delete(DBConnectionManager* manager, int no);
int KnowstandbyFile_Exists(DBConnectionManager* manager, int no);
int KnowstandbyFile_GetAll(DBConnectionManager* manager, KnowstandbyFile* records, int max_count, int* actual_count);

#endif /* KNOWSTANDBY_FILE_ORM_H */