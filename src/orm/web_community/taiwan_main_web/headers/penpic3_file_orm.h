#ifndef PENPIC3_FILE_ORM_H
#define PENPIC3_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} Penpic3File;

/* CRUD Operations */
int Penpic3File_Add(DBConnectionManager* manager, const Penpic3File* record);
int Penpic3File_Get(DBConnectionManager* manager, int no, Penpic3File* record);
int Penpic3File_Update(DBConnectionManager* manager, const Penpic3File* record);
int Penpic3File_Delete(DBConnectionManager* manager, int no);
int Penpic3File_Exists(DBConnectionManager* manager, int no);
int Penpic3File_GetAll(DBConnectionManager* manager, Penpic3File* records, int max_count, int* actual_count);

#endif /* PENPIC3_FILE_ORM_H */