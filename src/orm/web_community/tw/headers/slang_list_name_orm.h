#ifndef SLANG_LIST_NAME_ORM_H
#define SLANG_LIST_NAME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char slang[154];
} SlangListName;

/* CRUD Operations */
int SlangListName_Add(DBConnectionManager* manager, const SlangListName* record);
int SlangListName_Get(DBConnectionManager* manager, const char* slang, SlangListName* record);
int SlangListName_Update(DBConnectionManager* manager, const SlangListName* record);
int SlangListName_Delete(DBConnectionManager* manager, const char* slang);
int SlangListName_Exists(DBConnectionManager* manager, const char* slang);
int SlangListName_GetAll(DBConnectionManager* manager, SlangListName* records, int max_count, int* actual_count);

#endif /* SLANG_LIST_NAME_ORM_H */