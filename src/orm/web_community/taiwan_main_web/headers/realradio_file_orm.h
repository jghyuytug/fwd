#ifndef REALRADIO_FILE_ORM_H
#define REALRADIO_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} RealradioFile;

/* CRUD Operations */
int RealradioFile_Add(DBConnectionManager* manager, const RealradioFile* record);
int RealradioFile_Get(DBConnectionManager* manager, int no, RealradioFile* record);
int RealradioFile_Update(DBConnectionManager* manager, const RealradioFile* record);
int RealradioFile_Delete(DBConnectionManager* manager, int no);
int RealradioFile_Exists(DBConnectionManager* manager, int no);
int RealradioFile_GetAll(DBConnectionManager* manager, RealradioFile* records, int max_count, int* actual_count);

#endif /* REALRADIO_FILE_ORM_H */