#ifndef IMAGE_UPLOAD_ORM_H
#define IMAGE_UPLOAD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char date_start[11];
    char date_end[11];
    char img_url[201];
} ImageUpload;

/* CRUD Operations */
int ImageUpload_Add(DBConnectionManager* manager, const ImageUpload* record);
int ImageUpload_Get(DBConnectionManager* manager, int no, ImageUpload* record);
int ImageUpload_Update(DBConnectionManager* manager, const ImageUpload* record);
int ImageUpload_Delete(DBConnectionManager* manager, int no);
int ImageUpload_Exists(DBConnectionManager* manager, int no);
int ImageUpload_GetAll(DBConnectionManager* manager, ImageUpload* records, int max_count, int* actual_count);

#endif /* IMAGE_UPLOAD_ORM_H */