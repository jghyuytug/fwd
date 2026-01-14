#ifndef EVENT_ACCOUNT_FATIGUE_ORM_H
#define EVENT_ACCOUNT_FATIGUE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int fatigue;
} EventAccountFatigue;

/* CRUD Operations */
int EventAccountFatigue_Add(DBConnectionManager* manager, const EventAccountFatigue* record);
int EventAccountFatigue_Get(DBConnectionManager* manager, int m_id, EventAccountFatigue* record);
int EventAccountFatigue_Update(DBConnectionManager* manager, const EventAccountFatigue* record);
int EventAccountFatigue_Delete(DBConnectionManager* manager, int m_id);
int EventAccountFatigue_Exists(DBConnectionManager* manager, int m_id);
int EventAccountFatigue_GetAll(DBConnectionManager* manager, EventAccountFatigue* records, int max_count, int* actual_count);

#endif /* EVENT_ACCOUNT_FATIGUE_ORM_H */