#ifndef EVENT_NEWMEMBER0709_ENTRY_ORM_H
#define EVENT_NEWMEMBER0709_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned int occ_date;
    unsigned char server_id;
    int charac_no;
    unsigned int item1_no;
    unsigned int item1_check;
    unsigned int item2_no;
    unsigned int item2_check;
} EventNewmember0709Entry;

/* CRUD Operations */
int EventNewmember0709Entry_Add(DBConnectionManager* manager, const EventNewmember0709Entry* entry);
int EventNewmember0709Entry_Get(DBConnectionManager* manager, int m_id, EventNewmember0709Entry* entry);
int EventNewmember0709Entry_Update(DBConnectionManager* manager, const EventNewmember0709Entry* entry);
int EventNewmember0709Entry_Delete(DBConnectionManager* manager, int m_id);
int EventNewmember0709Entry_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int EventNewmember0709Entry_GetByDate(DBConnectionManager* manager, unsigned int occ_date,
                                       EventNewmember0709Entry* entries, int max_count, int* actual_count);
int EventNewmember0709Entry_GetByDateRange(DBConnectionManager* manager,
                                            unsigned int start_date, unsigned int end_date,
                                            EventNewmember0709Entry* entries, int max_count, int* actual_count);
int EventNewmember0709Entry_GetAll(DBConnectionManager* manager,
                                    EventNewmember0709Entry* entries, int max_count, int* actual_count);

/* Utility Functions */
void EventNewmember0709Entry_PrintInfo(const EventNewmember0709Entry* entry);

#endif
