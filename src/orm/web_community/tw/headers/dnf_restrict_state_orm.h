#ifndef DNF_RESTRICT_STATE_ORM_H
#define DNF_RESTRICT_STATE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int server_group;
    int category;
    int restrict_code;
    char restrict_value[2];
    char mod_date[20];
    char reg_date[20];
} DnfRestrictState;

/* CRUD Operations */
int DnfRestrictState_Add(DBConnectionManager* manager, const DnfRestrictState* record);
int DnfRestrictState_Get(DBConnectionManager* manager, int server_group, int category, int restrict_code, DnfRestrictState* record);
int DnfRestrictState_Update(DBConnectionManager* manager, const DnfRestrictState* record);
int DnfRestrictState_Delete(DBConnectionManager* manager, int server_group, int category, int restrict_code);
int DnfRestrictState_Exists(DBConnectionManager* manager, int server_group, int category, int restrict_code);
int DnfRestrictState_GetAll(DBConnectionManager* manager, DnfRestrictState* records, int max_count, int* actual_count);

#endif /* DNF_RESTRICT_STATE_ORM_H */