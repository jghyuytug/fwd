#ifndef EVENT_CLEAR_QUEST_ORM_H
#define EVENT_CLEAR_QUEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int clear_cnt;
} EventClearQuest;

/* CRUD Operations */
int EventClearQuest_Add(DBConnectionManager* manager, const EventClearQuest* record);
int EventClearQuest_Get(DBConnectionManager* manager, int m_id, EventClearQuest* record);
int EventClearQuest_Update(DBConnectionManager* manager, const EventClearQuest* record);
int EventClearQuest_Delete(DBConnectionManager* manager, int m_id);
int EventClearQuest_Exists(DBConnectionManager* manager, int m_id);
int EventClearQuest_GetAll(DBConnectionManager* manager, EventClearQuest* records, int max_count, int* actual_count);

#endif /* EVENT_CLEAR_QUEST_ORM_H */