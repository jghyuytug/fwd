#ifndef EVENT_NEWMEMBER0709_ENTRY_ORM_H
#define EVENT_NEWMEMBER0709_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_date;
    signed char server_id;
    int charac_no;
    int item1_no;
    int item1_check;
    int item2_no;
    int item2_check;
} EventNewmember0709Entry;

/* CRUD Operations */
int EventNewmember0709Entry_Add(DBConnectionManager* manager, const EventNewmember0709Entry* record);
int EventNewmember0709Entry_Get(DBConnectionManager* manager, int m_id, EventNewmember0709Entry* record);
int EventNewmember0709Entry_Update(DBConnectionManager* manager, const EventNewmember0709Entry* record);
int EventNewmember0709Entry_Delete(DBConnectionManager* manager, int m_id);
int EventNewmember0709Entry_Exists(DBConnectionManager* manager, int m_id);
int EventNewmember0709Entry_GetAll(DBConnectionManager* manager, EventNewmember0709Entry* records, int max_count, int* actual_count);

#endif /* EVENT_NEWMEMBER0709_ENTRY_ORM_H */