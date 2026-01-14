#ifndef EVENT_1112_ONTIME_USER_3_ORM_H
#define EVENT_1112_ONTIME_USER_3_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int last_no;
} Event1112OntimeUser3;

/* CRUD Operations */
int Event1112OntimeUser3_Add(DBConnectionManager* manager, const Event1112OntimeUser3* record);
int Event1112OntimeUser3_Get(DBConnectionManager* manager, int m_id, Event1112OntimeUser3* record);
int Event1112OntimeUser3_Update(DBConnectionManager* manager, const Event1112OntimeUser3* record);
int Event1112OntimeUser3_Delete(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser3_Exists(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser3_GetAll(DBConnectionManager* manager, Event1112OntimeUser3* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_USER_3_ORM_H */