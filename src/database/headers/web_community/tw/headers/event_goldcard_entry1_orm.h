#ifndef EVENT_GOLDCARD_ENTRY1_ORM_H
#define EVENT_GOLDCARD_ENTRY1_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int occ_date;
    int m_id;
    signed char item_no;
} EventGoldcardEntry1;

/* CRUD Operations */
int EventGoldcardEntry1_Add(DBConnectionManager* manager, const EventGoldcardEntry1* record);
int EventGoldcardEntry1_Get(DBConnectionManager* manager, int occ_date, int m_id, EventGoldcardEntry1* record);
int EventGoldcardEntry1_Update(DBConnectionManager* manager, const EventGoldcardEntry1* record);
int EventGoldcardEntry1_Delete(DBConnectionManager* manager, int occ_date, int m_id);
int EventGoldcardEntry1_Exists(DBConnectionManager* manager, int occ_date, int m_id);
int EventGoldcardEntry1_GetAll(DBConnectionManager* manager, EventGoldcardEntry1* records, int max_count, int* actual_count);

#endif /* EVENT_GOLDCARD_ENTRY1_ORM_H */