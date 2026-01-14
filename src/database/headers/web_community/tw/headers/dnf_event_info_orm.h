#ifndef DNF_EVENT_INFO_ORM_H
#define DNF_EVENT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    char event_name[31];
    char event_explain[101];
    signed char apply_type;
    char start_date[11];
    char end_date[11];
} DnfEventInfo;

/* CRUD Operations */
int DnfEventInfo_Add(DBConnectionManager* manager, const DnfEventInfo* record);
int DnfEventInfo_Get(DBConnectionManager* manager, int event_id, DnfEventInfo* record);
int DnfEventInfo_Update(DBConnectionManager* manager, const DnfEventInfo* record);
int DnfEventInfo_Delete(DBConnectionManager* manager, int event_id);
int DnfEventInfo_Exists(DBConnectionManager* manager, int event_id);
int DnfEventInfo_GetAll(DBConnectionManager* manager, DnfEventInfo* records, int max_count, int* actual_count);

#endif /* DNF_EVENT_INFO_ORM_H */