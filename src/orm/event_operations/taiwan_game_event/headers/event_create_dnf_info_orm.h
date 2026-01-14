#ifndef EVENT_CREATE_DNF_INFO_ORM_H
#define EVENT_CREATE_DNF_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_day;
    char occ_time[20];
    int rate;
} EventCreateDnfInfo;

/* CRUD Operations */
int EventCreateDnfInfo_Add(DBConnectionManager* manager, const EventCreateDnfInfo* record);
int EventCreateDnfInfo_Get(DBConnectionManager* manager, const char* occ_time, EventCreateDnfInfo* record);
int EventCreateDnfInfo_Update(DBConnectionManager* manager, const EventCreateDnfInfo* record);
int EventCreateDnfInfo_Delete(DBConnectionManager* manager, const char* occ_time);
int EventCreateDnfInfo_Exists(DBConnectionManager* manager, const char* occ_time);
int EventCreateDnfInfo_GetAll(DBConnectionManager* manager, EventCreateDnfInfo* records, int max_count, int* actual_count);

#endif /* EVENT_CREATE_DNF_INFO_ORM_H */