#ifndef EVENT_DNF_3YEAR_FEEL_ORM_H
#define EVENT_DNF_3YEAR_FEEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
} EventDnf3YearFeel;

/* CRUD Operations */
int EventDnf3YearFeel_Add(DBConnectionManager* manager, const EventDnf3YearFeel* record);
int EventDnf3YearFeel_Get(DBConnectionManager* manager, int no, int m_id, EventDnf3YearFeel* record);
int EventDnf3YearFeel_Update(DBConnectionManager* manager, const EventDnf3YearFeel* record);
int EventDnf3YearFeel_Delete(DBConnectionManager* manager, int no, int m_id);
int EventDnf3YearFeel_Exists(DBConnectionManager* manager, int no, int m_id);
int EventDnf3YearFeel_GetAll(DBConnectionManager* manager, EventDnf3YearFeel* records, int max_count, int* actual_count);

#endif /* EVENT_DNF_3YEAR_FEEL_ORM_H */