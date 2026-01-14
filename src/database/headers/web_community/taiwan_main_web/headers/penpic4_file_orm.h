#ifndef PENPIC4_FILE_ORM_H
#define PENPIC4_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} Penpic4File;

/* CRUD Operations */
int Penpic4File_Add(DBConnectionManager* manager, const Penpic4File* record);
int Penpic4File_Get(DBConnectionManager* manager, int no, Penpic4File* record);
int Penpic4File_Update(DBConnectionManager* manager, const Penpic4File* record);
int Penpic4File_Delete(DBConnectionManager* manager, int no);
int Penpic4File_Exists(DBConnectionManager* manager, int no);
int Penpic4File_GetAll(DBConnectionManager* manager, Penpic4File* records, int max_count, int* actual_count);

#endif /* PENPIC4_FILE_ORM_H */