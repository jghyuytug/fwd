#ifndef ZIPCODE_ORM_H
#define ZIPCODE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char zipcode[8];
    char sido[17];
    char gugun[31];
    char dong[61];
    char bunji[31];
} Zipcode;

/* CRUD Operations */
int Zipcode_Add(DBConnectionManager* manager, const Zipcode* record);
int Zipcode_GetAll(DBConnectionManager* manager, Zipcode* records, int max_count, int* actual_count);

#endif /* ZIPCODE_ORM_H */