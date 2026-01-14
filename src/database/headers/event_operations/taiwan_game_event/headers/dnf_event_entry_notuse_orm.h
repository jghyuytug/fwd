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
int DnfEventEntryNotuse_GetAll(DBConnectionManager* manager, DnfEventEntryNotuse* records, int max_count, int* actual_count);

#endif /* DNF_EVENT_ENTRY_NOTUSE_ORM_H */