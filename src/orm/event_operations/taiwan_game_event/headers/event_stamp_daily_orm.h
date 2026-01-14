#ifndef EVENT_STAMP_DAILY_ORM_H
#define EVENT_STAMP_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    short condition1;
    short condition2;
    short condition3;
} EventStampDaily;

/* CRUD Operations */
int EventStampDaily_Add(DBConnectionManager* manager, const EventStampDaily* record);
int EventStampDaily_Get(DBConnectionManager* manager, int charac_no, EventStampDaily* record);
int EventStampDaily_Update(DBConnectionManager* manager, const EventStampDaily* record);
int EventStampDaily_Delete(DBConnectionManager* manager, int charac_no);
int EventStampDaily_Exists(DBConnectionManager* manager, int charac_no);
int EventStampDaily_GetAll(DBConnectionManager* manager, EventStampDaily* records, int max_count, int* actual_count);

#endif /* EVENT_STAMP_DAILY_ORM_H */