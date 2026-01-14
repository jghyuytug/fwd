#ifndef EVENT_MAGE_2YEARS_ORM_H
#define EVENT_MAGE_2YEARS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_info;
    int charac_no;
    char charac_name[101];
    char create_time[20];
    char delete_time[20];
    signed char delete_flag;
} EventMage2Years;

/* CRUD Operations */
int EventMage2Years_Add(DBConnectionManager* manager, const EventMage2Years* record);
int EventMage2Years_Get(DBConnectionManager* manager, int m_id, signed char server_info, int charac_no, EventMage2Years* record);
int EventMage2Years_Update(DBConnectionManager* manager, const EventMage2Years* record);
int EventMage2Years_Delete(DBConnectionManager* manager, int m_id, signed char server_info, int charac_no);
int EventMage2Years_Exists(DBConnectionManager* manager, int m_id, signed char server_info, int charac_no);
int EventMage2Years_GetAll(DBConnectionManager* manager, EventMage2Years* records, int max_count, int* actual_count);

#endif /* EVENT_MAGE_2YEARS_ORM_H */