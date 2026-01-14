#ifndef PENPIC_FILE_ORM_H
#define PENPIC_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} PenpicFile;

/* CRUD Operations */
int PenpicFile_Add(DBConnectionManager* manager, const PenpicFile* record);
int PenpicFile_Get(DBConnectionManager* manager, int no, PenpicFile* record);
int PenpicFile_Update(DBConnectionManager* manager, const PenpicFile* record);
int PenpicFile_Delete(DBConnectionManager* manager, int no);
int PenpicFile_Exists(DBConnectionManager* manager, int no);
int PenpicFile_GetAll(DBConnectionManager* manager, PenpicFile* records, int max_count, int* actual_count);

#endif /* PENPIC_FILE_ORM_H */