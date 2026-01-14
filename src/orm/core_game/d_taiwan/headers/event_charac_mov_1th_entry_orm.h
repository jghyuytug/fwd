#ifndef EVENT_CHARAC_MOV_1TH_ENTRY_ORM_H
#define EVENT_CHARAC_MOV_1TH_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;
    int it_no;
    int item_check;
} EventCharacMov1thEntry;

/* CRUD Operations */
int EventCharacMov1thEntry_Add(DBConnectionManager* manager, const EventCharacMov1thEntry* entry);
int EventCharacMov1thEntry_Get(DBConnectionManager* manager, int m_id, EventCharacMov1thEntry* entry);
int EventCharacMov1thEntry_Update(DBConnectionManager* manager, const EventCharacMov1thEntry* entry);
int EventCharacMov1thEntry_Delete(DBConnectionManager* manager, int m_id);
int EventCharacMov1thEntry_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int EventCharacMov1thEntry_GetAll(DBConnectionManager* manager, EventCharacMov1thEntry* entries,
                                   int max_count, int* actual_count);
int EventCharacMov1thEntry_GetByItemNo(DBConnectionManager* manager, int it_no,
                                        EventCharacMov1thEntry* entries,
                                        int max_count, int* actual_count);

/* Utility Functions */
void EventCharacMov1thEntry_PrintInfo(const EventCharacMov1thEntry* entry);

#endif