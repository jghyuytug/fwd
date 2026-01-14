#ifndef LAG_STAT_MODULE_ORM_H
#define LAG_STAT_MODULE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_id;
    signed char module;
    int average;
    int deviation;
    int count;
} LagStatModule;

/* CRUD Operations */
int LagStatModule_Add(DBConnectionManager* manager, const LagStatModule* record);
int LagStatModule_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char module, LagStatModule* record);
int LagStatModule_Update(DBConnectionManager* manager, const LagStatModule* record);
int LagStatModule_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char module);
int LagStatModule_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char module);
int LagStatModule_GetAll(DBConnectionManager* manager, LagStatModule* records, int max_count, int* actual_count);

#endif /* LAG_STAT_MODULE_ORM_H */