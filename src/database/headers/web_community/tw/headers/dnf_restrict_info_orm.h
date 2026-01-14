#ifndef DNF_RESTRICT_INFO_ORM_H
#define DNF_RESTRICT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int category;
    int restrict_code;
    char restrict_str[46];
    char reg_date[20];
} DnfRestrictInfo;

/* CRUD Operations */
int DnfRestrictInfo_Add(DBConnectionManager* manager, const DnfRestrictInfo* record);
int DnfRestrictInfo_Get(DBConnectionManager* manager, int category, int restrict_code, DnfRestrictInfo* record);
int DnfRestrictInfo_Update(DBConnectionManager* manager, const DnfRestrictInfo* record);
int DnfRestrictInfo_Delete(DBConnectionManager* manager, int category, int restrict_code);
int DnfRestrictInfo_Exists(DBConnectionManager* manager, int category, int restrict_code);
int DnfRestrictInfo_GetAll(DBConnectionManager* manager, DnfRestrictInfo* records, int max_count, int* actual_count);

#endif /* DNF_RESTRICT_INFO_ORM_H */