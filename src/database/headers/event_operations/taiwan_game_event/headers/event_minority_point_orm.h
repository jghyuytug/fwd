#ifndef EVENT_MINORITY_POINT_ORM_H
#define EVENT_MINORITY_POINT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int point;
} EventMinorityPoint;

/* CRUD Operations */
int EventMinorityPoint_Add(DBConnectionManager* manager, const EventMinorityPoint* record);
int EventMinorityPoint_Get(DBConnectionManager* manager, int m_id, EventMinorityPoint* record);
int EventMinorityPoint_Update(DBConnectionManager* manager, const EventMinorityPoint* record);
int EventMinorityPoint_Delete(DBConnectionManager* manager, int m_id);
int EventMinorityPoint_Exists(DBConnectionManager* manager, int m_id);
int EventMinorityPoint_GetAll(DBConnectionManager* manager, EventMinorityPoint* records, int max_count, int* actual_count);

#endif /* EVENT_MINORITY_POINT_ORM_H */