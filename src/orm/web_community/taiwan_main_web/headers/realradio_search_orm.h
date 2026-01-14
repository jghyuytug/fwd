#ifndef REALRADIO_SEARCH_ORM_H
#define REALRADIO_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_type;
    signed char category;
    char class;
    char word[33];
} RealradioSearch;

/* CRUD Operations */
int RealradioSearch_Add(DBConnectionManager* manager, const RealradioSearch* record);
int RealradioSearch_GetAll(DBConnectionManager* manager, RealradioSearch* records, int max_count, int* actual_count);

#endif /* REALRADIO_SEARCH_ORM_H */