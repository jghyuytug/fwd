#ifndef EVENT_CHARAC_MOV_1TH_ENTRY_ORM_H
#define EVENT_CHARAC_MOV_1TH_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;
    int it_no;
    int item_check;
} EventCharacMov1ThEntry;

/* CRUD Operations */
int EventCharacMov1ThEntry_Add(DBConnectionManager* manager, const EventCharacMov1ThEntry* record);
int EventCharacMov1ThEntry_Get(DBConnectionManager* manager, int m_id, EventCharacMov1ThEntry* record);
int EventCharacMov1ThEntry_Update(DBConnectionManager* manager, const EventCharacMov1ThEntry* record);
int EventCharacMov1ThEntry_Delete(DBConnectionManager* manager, int m_id);
int EventCharacMov1ThEntry_Exists(DBConnectionManager* manager, int m_id);
int EventCharacMov1ThEntry_GetAll(DBConnectionManager* manager, EventCharacMov1ThEntry* records, int max_count, int* actual_count);

#endif /* EVENT_CHARAC_MOV_1TH_ENTRY_ORM_H */