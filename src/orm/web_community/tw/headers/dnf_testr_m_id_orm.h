#ifndef DNF_TESTR_M_ID_ORM_H
#define DNF_TESTR_M_ID_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char sex;
} DnfTestrMId;

/* CRUD Operations */
int DnfTestrMId_Add(DBConnectionManager* manager, const DnfTestrMId* record);
int DnfTestrMId_Get(DBConnectionManager* manager, int m_id, DnfTestrMId* record);
int DnfTestrMId_Update(DBConnectionManager* manager, const DnfTestrMId* record);
int DnfTestrMId_Delete(DBConnectionManager* manager, int m_id);
int DnfTestrMId_Exists(DBConnectionManager* manager, int m_id);
int DnfTestrMId_GetAll(DBConnectionManager* manager, DnfTestrMId* records, int max_count, int* actual_count);

#endif /* DNF_TESTR_M_ID_ORM_H */