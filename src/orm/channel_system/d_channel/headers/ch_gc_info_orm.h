#ifndef CH_GC_INFO_ORM_H
#define CH_GC_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char group_name[21];
    char group_gc_no[11];
} ChGcInfo;

/* CRUD Operations */
int ChGcInfo_Add(DBConnectionManager* manager, const ChGcInfo* record);
int ChGcInfo_GetAll(DBConnectionManager* manager, ChGcInfo* records, int max_count, int* actual_count);

#endif /* CH_GC_INFO_ORM_H */