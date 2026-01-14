#ifndef TEST2_ORM_H
#define TEST2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int a;
} Test2;

int Test2_Add(DBConnectionManager* manager, const Test2* record);
int Test2_Get(DBConnectionManager* manager, unsigned int a, Test2* record);
int Test2_Delete(DBConnectionManager* manager, unsigned int a);

#endif
