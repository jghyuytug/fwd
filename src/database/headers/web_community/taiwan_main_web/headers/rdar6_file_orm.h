#ifndef RDAR6_FILE_ORM_H
#define RDAR6_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} Rdar6File;

/* CRUD Operations */
int Rdar6File_Add(DBConnectionManager* manager, const Rdar6File* record);
int Rdar6File_Get(DBConnectionManager* manager, int no, Rdar6File* record);
int Rdar6File_Update(DBConnectionManager* manager, const Rdar6File* record);
int Rdar6File_Delete(DBConnectionManager* manager, int no);
int Rdar6File_Exists(DBConnectionManager* manager, int no);
int Rdar6File_GetAll(DBConnectionManager* manager, Rdar6File* records, int max_count, int* actual_count);

#endif /* RDAR6_FILE_ORM_H */