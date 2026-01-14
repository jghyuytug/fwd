#ifndef EVENT_CREATE_CHARAC_ORM_H
#define EVENT_CREATE_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
} EventCreateCharac;

/* CRUD Operations */
int EventCreateCharac_Add(DBConnectionManager* manager, const EventCreateCharac* record);
int EventCreateCharac_Get(DBConnectionManager* manager, int m_id, signed char server_id, EventCreateCharac* record);
int EventCreateCharac_Update(DBConnectionManager* manager, const EventCreateCharac* record);
int EventCreateCharac_Delete(DBConnectionManager* manager, int m_id, signed char server_id);
int EventCreateCharac_Exists(DBConnectionManager* manager, int m_id, signed char server_id);
int EventCreateCharac_GetAll(DBConnectionManager* manager, EventCreateCharac* records, int max_count, int* actual_count);

#endif /* EVENT_CREATE_CHARAC_ORM_H */