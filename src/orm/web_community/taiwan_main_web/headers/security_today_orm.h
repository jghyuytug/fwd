#ifndef SECURITY_TODAY_ORM_H
#define SECURITY_TODAY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char day[3];
    char contents[101];
    char link[256];
    signed char window_flag;
} SecurityToday;

/* CRUD Operations */
int SecurityToday_Add(DBConnectionManager* manager, const SecurityToday* record);
int SecurityToday_Get(DBConnectionManager* manager, int no, SecurityToday* record);
int SecurityToday_Update(DBConnectionManager* manager, const SecurityToday* record);
int SecurityToday_Delete(DBConnectionManager* manager, int no);
int SecurityToday_Exists(DBConnectionManager* manager, int no);
int SecurityToday_GetAll(DBConnectionManager* manager, SecurityToday* records, int max_count, int* actual_count);

#endif /* SECURITY_TODAY_ORM_H */