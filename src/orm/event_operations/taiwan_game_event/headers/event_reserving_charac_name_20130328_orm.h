#ifndef EVENT_RESERVING_CHARAC_NAME_20130328_ORM_H
#define EVENT_RESERVING_CHARAC_NAME_20130328_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char user_id[31];
    signed char server_info;
    char charac_name[21];
} EventReservingCharacName20130328;

/* CRUD Operations */
int EventReservingCharacName20130328_Add(DBConnectionManager* manager, const EventReservingCharacName20130328* record);
int EventReservingCharacName20130328_GetAll(DBConnectionManager* manager, EventReservingCharacName20130328* records, int max_count, int* actual_count);

#endif /* EVENT_RESERVING_CHARAC_NAME_20130328_ORM_H */