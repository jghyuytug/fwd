#ifndef REALRADIO_SEARCH_ETC_ORM_H
#define REALRADIO_SEARCH_ETC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_type;
    signed char category;
    char class;
    char word[33];
} RealradioSearchEtc;

/* CRUD Operations */
int RealradioSearchEtc_Add(DBConnectionManager* manager, const RealradioSearchEtc* record);
int RealradioSearchEtc_GetAll(DBConnectionManager* manager, RealradioSearchEtc* records, int max_count, int* actual_count);

#endif /* REALRADIO_SEARCH_ETC_ORM_H */