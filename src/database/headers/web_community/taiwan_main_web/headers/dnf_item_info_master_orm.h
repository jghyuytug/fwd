#ifndef DNF_ITEM_INFO_MASTER_ORM_H
#define DNF_ITEM_INFO_MASTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char master_no;
    short sub_no;
    char name[26];
    char master_explain[101];
} DnfItemInfoMaster;

/* CRUD Operations */
int DnfItemInfoMaster_Add(DBConnectionManager* manager, const DnfItemInfoMaster* record);
int DnfItemInfoMaster_Get(DBConnectionManager* manager, signed char master_no, short sub_no, DnfItemInfoMaster* record);
int DnfItemInfoMaster_Update(DBConnectionManager* manager, const DnfItemInfoMaster* record);
int DnfItemInfoMaster_Delete(DBConnectionManager* manager, signed char master_no, short sub_no);
int DnfItemInfoMaster_Exists(DBConnectionManager* manager, signed char master_no, short sub_no);
int DnfItemInfoMaster_GetAll(DBConnectionManager* manager, DnfItemInfoMaster* records, int max_count, int* actual_count);

#endif /* DNF_ITEM_INFO_MASTER_ORM_H */