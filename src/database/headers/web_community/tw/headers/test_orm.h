#ifndef TEST_ORM_H
#define TEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int a;
    char b[20];
} Test;

/* CRUD Operations */
int Test_Add(DBConnectionManager* manager, const Test* record);
int Test_GetAll(DBConnectionManager* manager, Test* records, int max_count, int* actual_count);

#endif /* TEST_ORM_H */