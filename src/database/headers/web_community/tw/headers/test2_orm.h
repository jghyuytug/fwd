#ifndef TEST2_ORM_H
#define TEST2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int a;
} Test2;

/* CRUD Operations */
int Test2_Add(DBConnectionManager* manager, const Test2* record);
int Test2_GetAll(DBConnectionManager* manager, Test2* records, int max_count, int* actual_count);

#endif /* TEST2_ORM_H */