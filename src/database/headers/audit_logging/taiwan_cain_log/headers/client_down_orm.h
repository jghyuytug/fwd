#ifndef CLIENT_DOWN_ORM_H
#define CLIENT_DOWN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int occ_count;
} ClientDown;

/* CRUD Operations */
int ClientDown_Add(DBConnectionManager* manager, const ClientDown* record);
int ClientDown_Get(DBConnectionManager* manager, const char* occ_date, ClientDown* record);
int ClientDown_Update(DBConnectionManager* manager, const ClientDown* record);
int ClientDown_Delete(DBConnectionManager* manager, const char* occ_date);
int ClientDown_Exists(DBConnectionManager* manager, const char* occ_date);
int ClientDown_GetAll(DBConnectionManager* manager, ClientDown* records, int max_count, int* actual_count);

#endif /* CLIENT_DOWN_ORM_H */