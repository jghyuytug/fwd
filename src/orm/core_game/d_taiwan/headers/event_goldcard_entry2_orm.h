#ifndef EVENT_GOLDCARD_ENTRY2_ORM_H
#define EVENT_GOLDCARD_ENTRY2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int occ_date;
    int m_id;
    unsigned char server_id;
    int charac_no;
    unsigned int item_no;
    int item_check;
} EventGoldcardEntry2;

/* CRUD Operations */
int EventGoldcardEntry2_Add(DBConnectionManager* manager, const EventGoldcardEntry2* entry);
int EventGoldcardEntry2_Get(DBConnectionManager* manager, int occ_date, int m_id, EventGoldcardEntry2* entry);
int EventGoldcardEntry2_Update(DBConnectionManager* manager, const EventGoldcardEntry2* entry);
int EventGoldcardEntry2_Delete(DBConnectionManager* manager, int occ_date, int m_id);
int EventGoldcardEntry2_Exists(DBConnectionManager* manager, int occ_date, int m_id);

/* Business Operations */
int EventGoldcardEntry2_GetByMId(DBConnectionManager* manager, int m_id,
                                  EventGoldcardEntry2* entries, int max_count, int* actual_count);
int EventGoldcardEntry2_GetByDate(DBConnectionManager* manager, int occ_date,
                                   EventGoldcardEntry2* entries, int max_count, int* actual_count);
int EventGoldcardEntry2_GetAll(DBConnectionManager* manager,
                                EventGoldcardEntry2* entries, int max_count, int* actual_count);

/* Utility Functions */
void EventGoldcardEntry2_PrintInfo(const EventGoldcardEntry2* entry);

#endif