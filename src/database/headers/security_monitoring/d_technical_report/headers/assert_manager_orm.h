#ifndef ASSERT_MANAGER_ORM_H
#define ASSERT_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char file_name[256];
    short file_line;
    char reason[256];
    int cnt;
} AssertManager;

/* CRUD Operations */
int AssertManager_Add(DBConnectionManager* manager, const AssertManager* record);
int AssertManager_Get(DBConnectionManager* manager, const char* file_name, short file_line, AssertManager* record);
int AssertManager_Update(DBConnectionManager* manager, const AssertManager* record);
int AssertManager_Delete(DBConnectionManager* manager, const char* file_name, short file_line);
int AssertManager_Exists(DBConnectionManager* manager, const char* file_name, short file_line);
int AssertManager_GetAll(DBConnectionManager* manager, AssertManager* records, int max_count, int* actual_count);

#endif /* ASSERT_MANAGER_ORM_H */