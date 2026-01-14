#ifndef EVENT_RESERVING_CHARAC_NAME_ORM_H
#define EVENT_RESERVING_CHARAC_NAME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char user_id[31];
    signed char server_info;
    char charac_name[21];
} EventReservingCharacName;

/* CRUD Operations */
int EventReservingCharacName_Add(DBConnectionManager* manager, const EventReservingCharacName* record);
int EventReservingCharacName_Get(DBConnectionManager* manager, signed char server_info, const char* charac_name, EventReservingCharacName* record);
int EventReservingCharacName_Update(DBConnectionManager* manager, const EventReservingCharacName* record);
int EventReservingCharacName_Delete(DBConnectionManager* manager, signed char server_info, const char* charac_name);
int EventReservingCharacName_Exists(DBConnectionManager* manager, signed char server_info, const char* charac_name);
int EventReservingCharacName_GetAll(DBConnectionManager* manager, EventReservingCharacName* records, int max_count, int* actual_count);

#endif /* EVENT_RESERVING_CHARAC_NAME_ORM_H */