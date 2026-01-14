#ifndef RDAR0_FILE_ORM_H
#define RDAR0_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} Rdar0File;

/* CRUD Operations */
int Rdar0File_Add(DBConnectionManager* manager, const Rdar0File* record);
int Rdar0File_Get(DBConnectionManager* manager, int no, Rdar0File* record);
int Rdar0File_Update(DBConnectionManager* manager, const Rdar0File* record);
int Rdar0File_Delete(DBConnectionManager* manager, int no);
int Rdar0File_Exists(DBConnectionManager* manager, int no);
int Rdar0File_GetAll(DBConnectionManager* manager, Rdar0File* records, int max_count, int* actual_count);

#endif /* RDAR0_FILE_ORM_H */