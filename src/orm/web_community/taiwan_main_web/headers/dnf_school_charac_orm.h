#ifndef DNF_SCHOOL_CHARAC_ORM_H
#define DNF_SCHOOL_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int school_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char occ_time[20];
} DnfSchoolCharac;

/* CRUD Operations */
int DnfSchoolCharac_Add(DBConnectionManager* manager, const DnfSchoolCharac* record);
int DnfSchoolCharac_Get(DBConnectionManager* manager, int m_id, DnfSchoolCharac* record);
int DnfSchoolCharac_Update(DBConnectionManager* manager, const DnfSchoolCharac* record);
int DnfSchoolCharac_Delete(DBConnectionManager* manager, int m_id);
int DnfSchoolCharac_Exists(DBConnectionManager* manager, int m_id);
int DnfSchoolCharac_GetAll(DBConnectionManager* manager, DnfSchoolCharac* records, int max_count, int* actual_count);

#endif /* DNF_SCHOOL_CHARAC_ORM_H */