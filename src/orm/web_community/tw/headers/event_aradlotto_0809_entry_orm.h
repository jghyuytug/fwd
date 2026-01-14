#ifndef EVENT_ARADLOTTO_0809_ENTRY_ORM_H
#define EVENT_ARADLOTTO_0809_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_date;
    char lotto_num[8];
} EventAradlotto0809Entry;

/* CRUD Operations */
int EventAradlotto0809Entry_Add(DBConnectionManager* manager, const EventAradlotto0809Entry* record);
int EventAradlotto0809Entry_Get(DBConnectionManager* manager, int m_id, EventAradlotto0809Entry* record);
int EventAradlotto0809Entry_Update(DBConnectionManager* manager, const EventAradlotto0809Entry* record);
int EventAradlotto0809Entry_Delete(DBConnectionManager* manager, int m_id);
int EventAradlotto0809Entry_Exists(DBConnectionManager* manager, int m_id);
int EventAradlotto0809Entry_GetAll(DBConnectionManager* manager, EventAradlotto0809Entry* records, int max_count, int* actual_count);

#endif /* EVENT_ARADLOTTO_0809_ENTRY_ORM_H */