#ifndef EVENT_RETURN_USER_ORM_H
#define EVENT_RETURN_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
} EventReturnUser;

/* CRUD Operations */
int EventReturnUser_Add(DBConnectionManager* manager, const EventReturnUser* record);
int EventReturnUser_Get(DBConnectionManager* manager, int m_id, EventReturnUser* record);
int EventReturnUser_Update(DBConnectionManager* manager, const EventReturnUser* record);
int EventReturnUser_Delete(DBConnectionManager* manager, int m_id);
int EventReturnUser_Exists(DBConnectionManager* manager, int m_id);
int EventReturnUser_GetAll(DBConnectionManager* manager, EventReturnUser* records, int max_count, int* actual_count);

#endif /* EVENT_RETURN_USER_ORM_H */