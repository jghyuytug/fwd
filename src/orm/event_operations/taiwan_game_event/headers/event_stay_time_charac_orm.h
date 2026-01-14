#ifndef EVENT_STAY_TIME_CHARAC_ORM_H
#define EVENT_STAY_TIME_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int reward_count;
    int reward_flag;
    char mod_date[20];
} EventStayTimeCharac;

/* CRUD Operations */
int EventStayTimeCharac_Add(DBConnectionManager* manager, const EventStayTimeCharac* record);
int EventStayTimeCharac_Get(DBConnectionManager* manager, int m_id, EventStayTimeCharac* record);
int EventStayTimeCharac_Update(DBConnectionManager* manager, const EventStayTimeCharac* record);
int EventStayTimeCharac_Delete(DBConnectionManager* manager, int m_id);
int EventStayTimeCharac_Exists(DBConnectionManager* manager, int m_id);
int EventStayTimeCharac_GetAll(DBConnectionManager* manager, EventStayTimeCharac* records, int max_count, int* actual_count);

#endif /* EVENT_STAY_TIME_CHARAC_ORM_H */