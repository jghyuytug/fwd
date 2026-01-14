#ifndef USED_MEMORY_ORM_H
#define USED_MEMORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    char minute_type[2];
    char module[2];
    short memory;
} UsedMemory;

/* CRUD Operations */
int UsedMemory_Add(DBConnectionManager* manager, const UsedMemory* record);
int UsedMemory_Get(DBConnectionManager* manager, const char* occ_time, const char* minute_type, const char* module, UsedMemory* record);
int UsedMemory_Update(DBConnectionManager* manager, const UsedMemory* record);
int UsedMemory_Delete(DBConnectionManager* manager, const char* occ_time, const char* minute_type, const char* module);
int UsedMemory_Exists(DBConnectionManager* manager, const char* occ_time, const char* minute_type, const char* module);
int UsedMemory_GetAll(DBConnectionManager* manager, UsedMemory* records, int max_count, int* actual_count);

#endif /* USED_MEMORY_ORM_H */