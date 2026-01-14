#ifndef UPDATE_ACT_CATEGORY_ORM_H
#define UPDATE_ACT_CATEGORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int act_no;
    signed char category_no;
    signed char order_no;
    char subject[41];
    char content[4096];
    char img_url[101];
} UpdateActCategory;

/* CRUD Operations */
int UpdateActCategory_Add(DBConnectionManager* manager, const UpdateActCategory* record);
int UpdateActCategory_Get(DBConnectionManager* manager, int act_no, signed char category_no, UpdateActCategory* record);
int UpdateActCategory_Update(DBConnectionManager* manager, const UpdateActCategory* record);
int UpdateActCategory_Delete(DBConnectionManager* manager, int act_no, signed char category_no);
int UpdateActCategory_Exists(DBConnectionManager* manager, int act_no, signed char category_no);
int UpdateActCategory_GetAll(DBConnectionManager* manager, UpdateActCategory* records, int max_count, int* actual_count);

#endif /* UPDATE_ACT_CATEGORY_ORM_H */