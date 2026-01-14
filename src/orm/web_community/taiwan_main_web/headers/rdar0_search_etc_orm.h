#ifndef RDAR0_SEARCH_ETC_ORM_H
#define RDAR0_SEARCH_ETC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} Rdar0SearchEtc;

/* CRUD Operations */
int Rdar0SearchEtc_Add(DBConnectionManager* manager, const Rdar0SearchEtc* record);
int Rdar0SearchEtc_GetAll(DBConnectionManager* manager, Rdar0SearchEtc* records, int max_count, int* actual_count);

#endif /* RDAR0_SEARCH_ETC_ORM_H */