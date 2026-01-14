#ifndef EVENT_STAMP_CHECKINFO_ORM_H
#define EVENT_STAMP_CHECKINFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    short current;
    unsigned char stamp_checkinfo[96];
} EventStampCheckinfo;

/* CRUD Operations */
int EventStampCheckinfo_Add(DBConnectionManager* manager, const EventStampCheckinfo* record);
int EventStampCheckinfo_Get(DBConnectionManager* manager, int charac_no, EventStampCheckinfo* record);
int EventStampCheckinfo_Update(DBConnectionManager* manager, const EventStampCheckinfo* record);
int EventStampCheckinfo_Delete(DBConnectionManager* manager, int charac_no);
int EventStampCheckinfo_Exists(DBConnectionManager* manager, int charac_no);
int EventStampCheckinfo_GetAll(DBConnectionManager* manager, EventStampCheckinfo* records, int max_count, int* actual_count);

#endif /* EVENT_STAMP_CHECKINFO_ORM_H */
