#ifndef LOG_HARDWARE_TING_ORM_H
#define LOG_HARDWARE_TING_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char category1;
    int category2;
    int category3;
    int total;
    int ting;
} LogHardwareTing;

/* CRUD Operations */
int LogHardwareTing_Add(DBConnectionManager* manager, const LogHardwareTing* record);
int LogHardwareTing_Get(DBConnectionManager* manager, const char* occ_time, signed char category1, int category2, int category3, LogHardwareTing* record);
int LogHardwareTing_Update(DBConnectionManager* manager, const LogHardwareTing* record);
int LogHardwareTing_Delete(DBConnectionManager* manager, const char* occ_time, signed char category1, int category2, int category3);
int LogHardwareTing_Exists(DBConnectionManager* manager, const char* occ_time, signed char category1, int category2, int category3);
int LogHardwareTing_GetAll(DBConnectionManager* manager, LogHardwareTing* records, int max_count, int* actual_count);

#endif /* LOG_HARDWARE_TING_ORM_H */