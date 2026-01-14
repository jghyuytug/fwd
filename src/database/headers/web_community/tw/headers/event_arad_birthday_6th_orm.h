#ifndef EVENT_ARAD_BIRTHDAY_6TH_ORM_H
#define EVENT_ARAD_BIRTHDAY_6TH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int server;
    int point;
} EventAradBirthday6Th;

/* CRUD Operations */
int EventAradBirthday6Th_Add(DBConnectionManager* manager, const EventAradBirthday6Th* record);
int EventAradBirthday6Th_Get(DBConnectionManager* manager, int server, EventAradBirthday6Th* record);
int EventAradBirthday6Th_Update(DBConnectionManager* manager, const EventAradBirthday6Th* record);
int EventAradBirthday6Th_Delete(DBConnectionManager* manager, int server);
int EventAradBirthday6Th_Exists(DBConnectionManager* manager, int server);
int EventAradBirthday6Th_GetAll(DBConnectionManager* manager, EventAradBirthday6Th* records, int max_count, int* actual_count);

#endif /* EVENT_ARAD_BIRTHDAY_6TH_ORM_H */