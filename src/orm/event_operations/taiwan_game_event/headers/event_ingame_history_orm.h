#ifndef EVENT_INGAME_HISTORY_ORM_H
#define EVENT_INGAME_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int type;
    int history_1;
    int history_2;
} EventIngameHistory;

/* CRUD Operations */
int EventIngameHistory_Add(DBConnectionManager* manager, const EventIngameHistory* record);
int EventIngameHistory_Get(DBConnectionManager* manager, int m_id, int type, EventIngameHistory* record);
int EventIngameHistory_Update(DBConnectionManager* manager, const EventIngameHistory* record);
int EventIngameHistory_Delete(DBConnectionManager* manager, int m_id, int type);
int EventIngameHistory_Exists(DBConnectionManager* manager, int m_id, int type);
int EventIngameHistory_GetAll(DBConnectionManager* manager, EventIngameHistory* records, int max_count, int* actual_count);

#endif /* EVENT_INGAME_HISTORY_ORM_H */