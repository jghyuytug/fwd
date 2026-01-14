#ifndef SLANG_LIST_NAME_ORM_H
#define SLANG_LIST_NAME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char slang[154];
} SlangListName;

/* CRUD Operations */
int SlangListName_Add(DBConnectionManager* manager, const char* slang);
int SlangListName_Exists(DBConnectionManager* manager, const char* slang);
int SlangListName_Delete(DBConnectionManager* manager, const char* slang);

/* Business Operations */
int SlangListName_GetAll(DBConnectionManager* manager, SlangListName* slang_list,
                         int max_count, int* actual_count);
int SlangListName_Count(DBConnectionManager* manager, int* count);
int SlangListName_Clear(DBConnectionManager* manager);

/* Utility Functions */
void SlangListName_PrintInfo(const SlangListName* slang);
int SlangListName_IsFiltered(DBConnectionManager* manager, const char* text);

#endif
