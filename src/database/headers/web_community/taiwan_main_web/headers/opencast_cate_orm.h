#ifndef OPENCAST_CATE_ORM_H
#define OPENCAST_CATE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int cate_no;
    char cate_name[51];
    char reg_date[11];
    char open_date[11];
} OpencastCate;

/* CRUD Operations */
int OpencastCate_Add(DBConnectionManager* manager, const OpencastCate* record);
int OpencastCate_Get(DBConnectionManager* manager, int cate_no, OpencastCate* record);
int OpencastCate_Update(DBConnectionManager* manager, const OpencastCate* record);
int OpencastCate_Delete(DBConnectionManager* manager, int cate_no);
int OpencastCate_Exists(DBConnectionManager* manager, int cate_no);
int OpencastCate_GetAll(DBConnectionManager* manager, OpencastCate* records, int max_count, int* actual_count);

#endif /* OPENCAST_CATE_ORM_H */