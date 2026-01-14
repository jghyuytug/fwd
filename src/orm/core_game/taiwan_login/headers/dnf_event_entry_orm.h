#ifndef DNF_EVENT_ENTRY_ORM_H
#define DNF_EVENT_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    int m_id;
    char occ_date[20];
    unsigned char server_id;
    int charac_no;
    char obtain_date[20];
} DnfEventEntry;

/* CRUD Operations */
int DnfEventEntry_Add(DBConnectionManager* manager, const DnfEventEntry* entry);
int DnfEventEntry_Get(DBConnectionManager* manager, int event_id, int m_id,
                       DnfEventEntry* entry);
int DnfEventEntry_Update(DBConnectionManager* manager, const DnfEventEntry* entry);
int DnfEventEntry_Delete(DBConnectionManager* manager, int event_id, int m_id);
int DnfEventEntry_Exists(DBConnectionManager* manager, int event_id, int m_id);

/* Business Operations */
int DnfEventEntry_GetByMemberId(DBConnectionManager* manager, int m_id,
                                 DnfEventEntry* entries, int max_count, int* actual_count);
int DnfEventEntry_GetByEventId(DBConnectionManager* manager, int event_id,
                                DnfEventEntry* entries, int max_count, int* actual_count);
int DnfEventEntry_GetByCharacter(DBConnectionManager* manager, int charac_no,
                                  DnfEventEntry* entries, int max_count, int* actual_count);
int DnfEventEntry_GetAll(DBConnectionManager* manager, DnfEventEntry* entries,
                          int max_count, int* actual_count);

/* Utility */
void DnfEventEntry_PrintInfo(const DnfEventEntry* entry);

#endif
