#ifndef EVENT_ONTIME_INFO_ORM_H
#define EVENT_ONTIME_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int item_index;
    int item_count;
    char time[20];
} EventOntimeInfo;

/* CRUD Operations */
int EventOntimeInfo_Add(DBConnectionManager* manager, const EventOntimeInfo* record);
int EventOntimeInfo_Get(DBConnectionManager* manager, int no, EventOntimeInfo* record);
int EventOntimeInfo_Update(DBConnectionManager* manager, const EventOntimeInfo* record);
int EventOntimeInfo_Delete(DBConnectionManager* manager, int no);
int EventOntimeInfo_Exists(DBConnectionManager* manager, int no);
int EventOntimeInfo_GetAll(DBConnectionManager* manager, EventOntimeInfo* records, int max_count, int* actual_count);

#endif /* EVENT_ONTIME_INFO_ORM_H */