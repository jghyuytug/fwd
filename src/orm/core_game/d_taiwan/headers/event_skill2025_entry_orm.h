#ifndef EVENT_SKILL2025_ENTRY_ORM_H
#define EVENT_SKILL2025_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned char server_id;
    int charac_no;
    char occ_time[20];
} EventSkill2025Entry;

/* CRUD Operations */
int EventSkill2025Entry_Add(DBConnectionManager* manager, const EventSkill2025Entry* entry);
int EventSkill2025Entry_Get(DBConnectionManager* manager, int m_id, EventSkill2025Entry* entry);
int EventSkill2025Entry_Update(DBConnectionManager* manager, const EventSkill2025Entry* entry);
int EventSkill2025Entry_Delete(DBConnectionManager* manager, int m_id);
int EventSkill2025Entry_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int EventSkill2025Entry_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                     EventSkill2025Entry* entries, int max_count, int* actual_count);
int EventSkill2025Entry_GetRecent(DBConnectionManager* manager, int limit,
                                   EventSkill2025Entry* entries, int max_count, int* actual_count);
int EventSkill2025Entry_GetAll(DBConnectionManager* manager,
                                EventSkill2025Entry* entries, int max_count, int* actual_count);

/* Utility Functions */
void EventSkill2025Entry_PrintInfo(const EventSkill2025Entry* entry);

#endif
