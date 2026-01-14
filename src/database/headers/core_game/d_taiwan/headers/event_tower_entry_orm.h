#ifndef EVENT_TOWER_ENTRY_ORM_H
#define EVENT_TOWER_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned int occ_date;
    unsigned int occ_check;
    char server_id;
    int charac_no;
    unsigned int item1_no;
    unsigned int item1_check;
    unsigned int item2_no;
    unsigned int item2_check;
    unsigned int item3_no;
    unsigned int item3_check;
} EventTowerEntry;

/* CRUD Operations */
int EventTowerEntry_Add(DBConnectionManager* manager, const EventTowerEntry* entry);
int EventTowerEntry_Get(DBConnectionManager* manager, int m_id, EventTowerEntry* entry);
int EventTowerEntry_Update(DBConnectionManager* manager, const EventTowerEntry* entry);
int EventTowerEntry_Delete(DBConnectionManager* manager, int m_id);
int EventTowerEntry_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int EventTowerEntry_GetByDateRange(DBConnectionManager* manager, unsigned int start_date,
                                    unsigned int end_date, EventTowerEntry* entries,
                                    int max_count, int* actual_count);
int EventTowerEntry_GetByServerId(DBConnectionManager* manager, char server_id,
                                   EventTowerEntry* entries, int max_count, int* actual_count);
int EventTowerEntry_GetAll(DBConnectionManager* manager, EventTowerEntry* entries,
                            int max_count, int* actual_count);
int EventTowerEntry_CheckItem(DBConnectionManager* manager, int m_id, int item_index,
                               unsigned int* item_no, unsigned int* item_check);

/* Utility Functions */
void EventTowerEntry_PrintInfo(const EventTowerEntry* entry);

#endif
