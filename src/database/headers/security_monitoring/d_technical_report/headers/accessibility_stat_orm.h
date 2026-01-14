#ifndef ACCESSIBILITY_STAT_ORM_H
#define ACCESSIBILITY_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char main_type;
    short sub_type;
    int val;
} AccessibilityStat;

/* CRUD Operations */
int AccessibilityStat_Add(DBConnectionManager* manager, const AccessibilityStat* record);
int AccessibilityStat_Get(DBConnectionManager* manager, const char* occ_time, signed char main_type, short sub_type, AccessibilityStat* record);
int AccessibilityStat_Update(DBConnectionManager* manager, const AccessibilityStat* record);
int AccessibilityStat_Delete(DBConnectionManager* manager, const char* occ_time, signed char main_type, short sub_type);
int AccessibilityStat_Exists(DBConnectionManager* manager, const char* occ_time, signed char main_type, short sub_type);
int AccessibilityStat_GetAll(DBConnectionManager* manager, AccessibilityStat* records, int max_count, int* actual_count);

#endif /* ACCESSIBILITY_STAT_ORM_H */