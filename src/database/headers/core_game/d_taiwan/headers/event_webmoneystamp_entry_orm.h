#ifndef EVENT_WEBMONEYSTAMP_ENTRY_ORM_H
#define EVENT_WEBMONEYSTAMP_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    unsigned short attend_point;
    char last_attend_time[20];
    char return_flag;
    char entry_item;
} EventWebmoneystampEntry;

/* CRUD Operations */
int EventWebmoneystampEntry_Add(DBConnectionManager* manager, const EventWebmoneystampEntry* entry);
int EventWebmoneystampEntry_Get(DBConnectionManager* manager, int m_id, EventWebmoneystampEntry* entry);
int EventWebmoneystampEntry_Update(DBConnectionManager* manager, const EventWebmoneystampEntry* entry);
int EventWebmoneystampEntry_Delete(DBConnectionManager* manager, int m_id);
int EventWebmoneystampEntry_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int EventWebmoneystampEntry_GetByReturnFlag(DBConnectionManager* manager, char return_flag,
                                             EventWebmoneystampEntry* entries, int max_count, int* actual_count);
int EventWebmoneystampEntry_GetByPointRange(DBConnectionManager* manager,
                                              unsigned short min_point, unsigned short max_point,
                                              EventWebmoneystampEntry* entries, int max_count, int* actual_count);
int EventWebmoneystampEntry_GetAll(DBConnectionManager* manager,
                                    EventWebmoneystampEntry* entries, int max_count, int* actual_count);

/* Utility Functions */
void EventWebmoneystampEntry_PrintInfo(const EventWebmoneystampEntry* entry);

#endif
