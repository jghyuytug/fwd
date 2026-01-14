#ifndef EVENT_ONTIME_USER_ORM_H
#define EVENT_ONTIME_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int last_no;
} EventOntimeUser;

/* CRUD Operations */
int EventOntimeUser_Add(DBConnectionManager* manager, const EventOntimeUser* record);
int EventOntimeUser_Get(DBConnectionManager* manager, int m_id, EventOntimeUser* record);
int EventOntimeUser_Update(DBConnectionManager* manager, const EventOntimeUser* record);
int EventOntimeUser_Delete(DBConnectionManager* manager, int m_id);
int EventOntimeUser_Exists(DBConnectionManager* manager, int m_id);
int EventOntimeUser_GetAll(DBConnectionManager* manager, EventOntimeUser* records, int max_count, int* actual_count);

#endif /* EVENT_ONTIME_USER_ORM_H */