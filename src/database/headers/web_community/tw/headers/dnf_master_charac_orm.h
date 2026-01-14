#ifndef DNF_MASTER_CHARAC_ORM_H
#define DNF_MASTER_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char global_type;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char lev;
} DnfMasterCharac;

/* CRUD Operations */
int DnfMasterCharac_Add(DBConnectionManager* manager, const DnfMasterCharac* record);
int DnfMasterCharac_Get(DBConnectionManager* manager, int m_id, signed char global_type, signed char server_id, DnfMasterCharac* record);
int DnfMasterCharac_Update(DBConnectionManager* manager, const DnfMasterCharac* record);
int DnfMasterCharac_Delete(DBConnectionManager* manager, int m_id, signed char global_type, signed char server_id);
int DnfMasterCharac_Exists(DBConnectionManager* manager, int m_id, signed char global_type, signed char server_id);
int DnfMasterCharac_GetAll(DBConnectionManager* manager, DnfMasterCharac* records, int max_count, int* actual_count);

#endif /* DNF_MASTER_CHARAC_ORM_H */