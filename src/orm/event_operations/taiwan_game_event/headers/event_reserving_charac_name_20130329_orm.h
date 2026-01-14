#ifndef EVENT_RESERVING_CHARAC_NAME_20130329_ORM_H
#define EVENT_RESERVING_CHARAC_NAME_20130329_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char user_id[31];
    signed char server_info;
    char charac_name[21];
} EventReservingCharacName20130329;

/* CRUD Operations */
int EventReservingCharacName20130329_Add(DBConnectionManager* manager, const EventReservingCharacName20130329* record);
int EventReservingCharacName20130329_Get(DBConnectionManager* manager, signed char server_info, const char* charac_name, EventReservingCharacName20130329* record);
int EventReservingCharacName20130329_Update(DBConnectionManager* manager, const EventReservingCharacName20130329* record);
int EventReservingCharacName20130329_Delete(DBConnectionManager* manager, signed char server_info, const char* charac_name);
int EventReservingCharacName20130329_Exists(DBConnectionManager* manager, signed char server_info, const char* charac_name);
int EventReservingCharacName20130329_GetAll(DBConnectionManager* manager, EventReservingCharacName20130329* records, int max_count, int* actual_count);

#endif /* EVENT_RESERVING_CHARAC_NAME_20130329_ORM_H */