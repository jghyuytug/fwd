#ifndef SLANG_LIST_ORM_H
#define SLANG_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char slang[154];
} SlangList;

/* CRUD Operations */
int SlangList_Add(DBConnectionManager* manager, const char* slang);
int SlangList_Exists(DBConnectionManager* manager, const char* slang);
int SlangList_Delete(DBConnectionManager* manager, const char* slang);

/* Business Operations */
int SlangList_GetAll(DBConnectionManager* manager, SlangList* slang_list,
                     int max_count, int* actual_count);
int SlangList_Count(DBConnectionManager* manager, int* count);
int SlangList_Clear(DBConnectionManager* manager);

/* Utility Functions */
void SlangList_PrintInfo(const SlangList* slang);
int SlangList_IsFiltered(DBConnectionManager* manager, const char* text);

#endif
