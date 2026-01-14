#ifndef EVENT_GOLDCARD_LOG_ORM_H
#define EVENT_GOLDCARD_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_date[11];
    short cnt;
} EventGoldcardLog;

/* CRUD Operations */
int EventGoldcardLog_Add(DBConnectionManager* manager, const EventGoldcardLog* record);
int EventGoldcardLog_Get(DBConnectionManager* manager, int m_id, const char* occ_date, EventGoldcardLog* record);
int EventGoldcardLog_Update(DBConnectionManager* manager, const EventGoldcardLog* record);
int EventGoldcardLog_Delete(DBConnectionManager* manager, int m_id, const char* occ_date);
int EventGoldcardLog_Exists(DBConnectionManager* manager, int m_id, const char* occ_date);
int EventGoldcardLog_GetAll(DBConnectionManager* manager, EventGoldcardLog* records, int max_count, int* actual_count);

#endif /* EVENT_GOLDCARD_LOG_ORM_H */