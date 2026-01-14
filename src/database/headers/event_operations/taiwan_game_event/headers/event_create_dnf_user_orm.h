#ifndef EVENT_CREATE_DNF_USER_ORM_H
#define EVENT_CREATE_DNF_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int m_id;
    signed char server_id;
    char occ_time[9];
} EventCreateDnfUser;

/* CRUD Operations */
int EventCreateDnfUser_Add(DBConnectionManager* manager, const EventCreateDnfUser* record);
int EventCreateDnfUser_Get(DBConnectionManager* manager, const char* occ_date, int m_id, EventCreateDnfUser* record);
int EventCreateDnfUser_Update(DBConnectionManager* manager, const EventCreateDnfUser* record);
int EventCreateDnfUser_Delete(DBConnectionManager* manager, const char* occ_date, int m_id);
int EventCreateDnfUser_Exists(DBConnectionManager* manager, const char* occ_date, int m_id);
int EventCreateDnfUser_GetAll(DBConnectionManager* manager, EventCreateDnfUser* records, int max_count, int* actual_count);

#endif /* EVENT_CREATE_DNF_USER_ORM_H */