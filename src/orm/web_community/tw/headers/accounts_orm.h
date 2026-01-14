#ifndef ACCOUNTS_ORM_H
#define ACCOUNTS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int UID;
    char accountname[256];
    char password[256];
} Accounts;

/* CRUD Operations */
int Accounts_Add(DBConnectionManager* manager, const Accounts* record);
int Accounts_Get(DBConnectionManager* manager, int UID, Accounts* record);
int Accounts_Update(DBConnectionManager* manager, const Accounts* record);
int Accounts_Delete(DBConnectionManager* manager, int UID);
int Accounts_Exists(DBConnectionManager* manager, int UID);
int Accounts_GetAll(DBConnectionManager* manager, Accounts* records, int max_count, int* actual_count);

#endif /* ACCOUNTS_ORM_H */