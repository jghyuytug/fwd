#ifndef STORE_ORM_H
#define STORE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    signed char use_doll;
} Store;

/* CRUD Operations */
int Store_Add(DBConnectionManager* manager, const Store* record);
int Store_Get(DBConnectionManager* manager, int charac_no, Store* record);
int Store_Update(DBConnectionManager* manager, const Store* record);
int Store_Delete(DBConnectionManager* manager, int charac_no);
int Store_Exists(DBConnectionManager* manager, int charac_no);
int Store_GetAll(DBConnectionManager* manager, Store* records, int max_count, int* actual_count);

#endif /* STORE_ORM_H */