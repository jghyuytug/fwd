#ifndef EVENT_1112_ONTIME_USER_5_ORM_H
#define EVENT_1112_ONTIME_USER_5_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int last_no;
} Event1112OntimeUser5;

/* CRUD Operations */
int Event1112OntimeUser5_Add(DBConnectionManager* manager, const Event1112OntimeUser5* record);
int Event1112OntimeUser5_Get(DBConnectionManager* manager, int m_id, Event1112OntimeUser5* record);
int Event1112OntimeUser5_Update(DBConnectionManager* manager, const Event1112OntimeUser5* record);
int Event1112OntimeUser5_Delete(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser5_Exists(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser5_GetAll(DBConnectionManager* manager, Event1112OntimeUser5* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_USER_5_ORM_H */