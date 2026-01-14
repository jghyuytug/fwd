#ifndef EVENT_1112_ONTIME_USER_1_ORM_H
#define EVENT_1112_ONTIME_USER_1_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int last_no;
} Event1112OntimeUser1;

/* CRUD Operations */
int Event1112OntimeUser1_Add(DBConnectionManager* manager, const Event1112OntimeUser1* record);
int Event1112OntimeUser1_Get(DBConnectionManager* manager, int m_id, Event1112OntimeUser1* record);
int Event1112OntimeUser1_Update(DBConnectionManager* manager, const Event1112OntimeUser1* record);
int Event1112OntimeUser1_Delete(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser1_Exists(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser1_GetAll(DBConnectionManager* manager, Event1112OntimeUser1* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_USER_1_ORM_H */