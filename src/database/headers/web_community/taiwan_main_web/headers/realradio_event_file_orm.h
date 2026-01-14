#ifndef REALRADIO_EVENT_FILE_ORM_H
#define REALRADIO_EVENT_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} RealradioEventFile;

/* CRUD Operations */
int RealradioEventFile_Add(DBConnectionManager* manager, const RealradioEventFile* record);
int RealradioEventFile_Get(DBConnectionManager* manager, int no, RealradioEventFile* record);
int RealradioEventFile_Update(DBConnectionManager* manager, const RealradioEventFile* record);
int RealradioEventFile_Delete(DBConnectionManager* manager, int no);
int RealradioEventFile_Exists(DBConnectionManager* manager, int no);
int RealradioEventFile_GetAll(DBConnectionManager* manager, RealradioEventFile* records, int max_count, int* actual_count);

#endif /* REALRADIO_EVENT_FILE_ORM_H */