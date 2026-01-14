#ifndef EVENT_1112_ONTIME_USER_8_ORM_H
#define EVENT_1112_ONTIME_USER_8_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int last_no;
} Event1112OntimeUser8;

/* CRUD Operations */
int Event1112OntimeUser8_Add(DBConnectionManager* manager, const Event1112OntimeUser8* record);
int Event1112OntimeUser8_Get(DBConnectionManager* manager, int m_id, Event1112OntimeUser8* record);
int Event1112OntimeUser8_Update(DBConnectionManager* manager, const Event1112OntimeUser8* record);
int Event1112OntimeUser8_Delete(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser8_Exists(DBConnectionManager* manager, int m_id);
int Event1112OntimeUser8_GetAll(DBConnectionManager* manager, Event1112OntimeUser8* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_USER_8_ORM_H */