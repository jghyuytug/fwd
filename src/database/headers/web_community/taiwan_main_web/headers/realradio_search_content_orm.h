#ifndef REALRADIO_SEARCH_CONTENT_ORM_H
#define REALRADIO_SEARCH_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_type;
    signed char category;
    char class;
    char word[33];
} RealradioSearchContent;

/* CRUD Operations */
int RealradioSearchContent_Add(DBConnectionManager* manager, const RealradioSearchContent* record);
int RealradioSearchContent_GetAll(DBConnectionManager* manager, RealradioSearchContent* records, int max_count, int* actual_count);

#endif /* REALRADIO_SEARCH_CONTENT_ORM_H */