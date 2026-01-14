#ifndef TOWNTREEINFOTABLE_ORM_H
#define TOWNTREEINFOTABLE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    char Items[8192];
} Towntreeinfotable;

/* CRUD Operations */
int Towntreeinfotable_Add(DBConnectionManager* manager, const Towntreeinfotable* record);
int Towntreeinfotable_Get(DBConnectionManager* manager, int Id, Towntreeinfotable* record);
int Towntreeinfotable_Update(DBConnectionManager* manager, const Towntreeinfotable* record);
int Towntreeinfotable_Delete(DBConnectionManager* manager, int Id);
int Towntreeinfotable_Exists(DBConnectionManager* manager, int Id);
int Towntreeinfotable_GetAll(DBConnectionManager* manager, Towntreeinfotable* records, int max_count, int* actual_count);

#endif /* TOWNTREEINFOTABLE_ORM_H */