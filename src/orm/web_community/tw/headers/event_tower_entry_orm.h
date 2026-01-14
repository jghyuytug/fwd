#ifndef EVENT_TOWER_ENTRY_ORM_H
#define EVENT_TOWER_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_date;
    int occ_check;
    signed char server_id;
    int charac_no;
    int item1_no;
    int item1_check;
    int item2_no;
    int item2_check;
    int item3_no;
    int item3_check;
} EventTowerEntry;

/* CRUD Operations */
int EventTowerEntry_Add(DBConnectionManager* manager, const EventTowerEntry* record);
int EventTowerEntry_Get(DBConnectionManager* manager, int m_id, EventTowerEntry* record);
int EventTowerEntry_Update(DBConnectionManager* manager, const EventTowerEntry* record);
int EventTowerEntry_Delete(DBConnectionManager* manager, int m_id);
int EventTowerEntry_Exists(DBConnectionManager* manager, int m_id);
int EventTowerEntry_GetAll(DBConnectionManager* manager, EventTowerEntry* records, int max_count, int* actual_count);

#endif /* EVENT_TOWER_ENTRY_ORM_H */