#ifndef EVENT_1112_ONTIME_INFO_ORM_H
#define EVENT_1112_ONTIME_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int item_index;
    int item_count;
    char time[20];
} Event1112OntimeInfo;

/* CRUD Operations */
int Event1112OntimeInfo_Add(DBConnectionManager* manager, const Event1112OntimeInfo* record);
int Event1112OntimeInfo_Get(DBConnectionManager* manager, int no, Event1112OntimeInfo* record);
int Event1112OntimeInfo_Update(DBConnectionManager* manager, const Event1112OntimeInfo* record);
int Event1112OntimeInfo_Delete(DBConnectionManager* manager, int no);
int Event1112OntimeInfo_Exists(DBConnectionManager* manager, int no);
int Event1112OntimeInfo_GetAll(DBConnectionManager* manager, Event1112OntimeInfo* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_INFO_ORM_H */