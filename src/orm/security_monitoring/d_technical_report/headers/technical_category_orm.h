#ifndef TECHNICAL_CATEGORY_ORM_H
#define TECHNICAL_CATEGORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int cateno;
    int pcateno;
    char name[101];
    int step;
} TechnicalCategory;

/* CRUD Operations */
int TechnicalCategory_Add(DBConnectionManager* manager, const TechnicalCategory* record);
int TechnicalCategory_GetAll(DBConnectionManager* manager, TechnicalCategory* records, int max_count, int* actual_count);

#endif /* TECHNICAL_CATEGORY_ORM_H */