#ifndef NEWMAGAZINE_IMAGE_UPLOAD_ORM_H
#define NEWMAGAZINE_IMAGE_UPLOAD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char date_start[11];
    char date_end[11];
    char img_url[201];
} NewmagazineImageUpload;

/* CRUD Operations */
int NewmagazineImageUpload_Add(DBConnectionManager* manager, const NewmagazineImageUpload* record);
int NewmagazineImageUpload_Get(DBConnectionManager* manager, int no, NewmagazineImageUpload* record);
int NewmagazineImageUpload_Update(DBConnectionManager* manager, const NewmagazineImageUpload* record);
int NewmagazineImageUpload_Delete(DBConnectionManager* manager, int no);
int NewmagazineImageUpload_Exists(DBConnectionManager* manager, int no);
int NewmagazineImageUpload_GetAll(DBConnectionManager* manager, NewmagazineImageUpload* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_IMAGE_UPLOAD_ORM_H */