#ifndef SPEC_INFO_ORM_H
#define SPEC_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int uid;
    int vendor_id;
    int device_id;
    char vendor_name[51];
    char device_name[121];
} SpecInfo;

/* CRUD Operations */
int SpecInfo_Add(DBConnectionManager* manager, const SpecInfo* record);
int SpecInfo_Get(DBConnectionManager* manager, int uid, SpecInfo* record);
int SpecInfo_Update(DBConnectionManager* manager, const SpecInfo* record);
int SpecInfo_Delete(DBConnectionManager* manager, int uid);
int SpecInfo_Exists(DBConnectionManager* manager, int uid);
int SpecInfo_GetAll(DBConnectionManager* manager, SpecInfo* records, int max_count, int* actual_count);

#endif /* SPEC_INFO_ORM_H */