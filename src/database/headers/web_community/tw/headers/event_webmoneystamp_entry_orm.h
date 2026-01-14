#ifndef EVENT_WEBMONEYSTAMP_ENTRY_ORM_H
#define EVENT_WEBMONEYSTAMP_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    short attend_point;
    char last_attend_time[20];
    signed char return_flag;
    signed char entry_item;
} EventWebmoneystampEntry;

/* CRUD Operations */
int EventWebmoneystampEntry_Add(DBConnectionManager* manager, const EventWebmoneystampEntry* record);
int EventWebmoneystampEntry_Get(DBConnectionManager* manager, int m_id, EventWebmoneystampEntry* record);
int EventWebmoneystampEntry_Update(DBConnectionManager* manager, const EventWebmoneystampEntry* record);
int EventWebmoneystampEntry_Delete(DBConnectionManager* manager, int m_id);
int EventWebmoneystampEntry_Exists(DBConnectionManager* manager, int m_id);
int EventWebmoneystampEntry_GetAll(DBConnectionManager* manager, EventWebmoneystampEntry* records, int max_count, int* actual_count);

#endif /* EVENT_WEBMONEYSTAMP_ENTRY_ORM_H */