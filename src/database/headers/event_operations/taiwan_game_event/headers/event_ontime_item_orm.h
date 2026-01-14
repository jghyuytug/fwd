#ifndef EVENT_ONTIME_ITEM_ORM_H
#define EVENT_ONTIME_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    int cnt;
} EventOntimeItem;

/* CRUD Operations */
int EventOntimeItem_Add(DBConnectionManager* manager, const EventOntimeItem* record);
int EventOntimeItem_Get(DBConnectionManager* manager, int idx, EventOntimeItem* record);
int EventOntimeItem_Update(DBConnectionManager* manager, const EventOntimeItem* record);
int EventOntimeItem_Delete(DBConnectionManager* manager, int idx);
int EventOntimeItem_Exists(DBConnectionManager* manager, int idx);
int EventOntimeItem_GetAll(DBConnectionManager* manager, EventOntimeItem* records, int max_count, int* actual_count);

#endif /* EVENT_ONTIME_ITEM_ORM_H */