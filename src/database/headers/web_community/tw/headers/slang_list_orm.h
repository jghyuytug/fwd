#ifndef SLANG_LIST_ORM_H
#define SLANG_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char slang[154];
} SlangList;

/* CRUD Operations */
int SlangList_Add(DBConnectionManager* manager, const SlangList* record);
int SlangList_Get(DBConnectionManager* manager, const char* slang, SlangList* record);
int SlangList_Update(DBConnectionManager* manager, const SlangList* record);
int SlangList_Delete(DBConnectionManager* manager, const char* slang);
int SlangList_Exists(DBConnectionManager* manager, const char* slang);
int SlangList_GetAll(DBConnectionManager* manager, SlangList* records, int max_count, int* actual_count);

#endif /* SLANG_LIST_ORM_H */