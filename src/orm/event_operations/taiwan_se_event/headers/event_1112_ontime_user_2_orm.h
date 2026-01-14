#ifndef EVENT_1112_ONTIME_USER_2_ORM_H
#define EVENT_1112_ONTIME_USER_2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int last_no;
} Event1112OntimeUser2;

/* CRUD Operations */
int Event1112OntimeUser2_Add(DBConnectionManager* manager, const Event1112OntimeUser2* record);
int Event1112OntimeUser2_Get(DBConnectionManager* manager, int m_id, Event1112OntimeUser2* record);
int Event1112OntimeUser2_Update(DBConnectionManager* manager, const Event1112OntimeUser2* record);
int Event1112OntimeUser2_Delete(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser2_Exists(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser2_GetAll(DBConnectionManager* manager, Event1112OntimeUser2* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_USER_2_ORM_H */