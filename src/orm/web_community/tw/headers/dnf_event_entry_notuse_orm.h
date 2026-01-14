#ifndef DNF_EVENT_ENTRY_NOTUSE_ORM_H
#define DNF_EVENT_ENTRY_NOTUSE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    int m_id;
    char occ_date[20];
    signed char server_id;
    int charac_no;
    char obtain_date[20];
} DnfEventEntryNotuse;

/* CRUD Operations */
int DnfEventEntryNotuse_Add(DBConnectionManager* manager, const DnfEventEntryNotuse* record);
int DnfEventEntryNotuse_Get(DBConnectionManager* manager, int event_id, int m_id, DnfEventEntryNotuse* record);
int DnfEventEntryNotuse_Update(DBConnectionManager* manager, const DnfEventEntryNotuse* record);
int DnfEventEntryNotuse_Delete(DBConnectionManager* manager, int event_id, int m_id);
int DnfEventEntryNotuse_Exists(DBConnectionManager* manager, int event_id, int m_id);
int DnfEventEntryNotuse_GetAll(DBConnectionManager* manager, DnfEventEntryNotuse* records, int max_count, int* actual_count);

#endif /* DNF_EVENT_ENTRY_NOTUSE_ORM_H */