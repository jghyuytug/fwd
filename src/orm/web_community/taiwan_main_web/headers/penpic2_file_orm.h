#ifndef PENPIC2_FILE_ORM_H
#define PENPIC2_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} Penpic2File;

/* CRUD Operations */
int Penpic2File_Add(DBConnectionManager* manager, const Penpic2File* record);
int Penpic2File_Get(DBConnectionManager* manager, int no, Penpic2File* record);
int Penpic2File_Update(DBConnectionManager* manager, const Penpic2File* record);
int Penpic2File_Delete(DBConnectionManager* manager, int no);
int Penpic2File_Exists(DBConnectionManager* manager, int no);
int Penpic2File_GetAll(DBConnectionManager* manager, Penpic2File* records, int max_count, int* actual_count);

#endif /* PENPIC2_FILE_ORM_H */