#ifndef EVENT_1112_AT_MAGE_12UP_ORM_H
#define EVENT_1112_AT_MAGE_12UP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    signed char server_id;
    char occ_date[20];
} Event1112AtMage12Up;

/* CRUD Operations */
int Event1112AtMage12Up_Add(DBConnectionManager* manager, const Event1112AtMage12Up* record);
int Event1112AtMage12Up_Get(DBConnectionManager* manager, int m_id, Event1112AtMage12Up* record);
int Event1112AtMage12Up_Update(DBConnectionManager* manager, const Event1112AtMage12Up* record);
int Event1112AtMage12Up_Delete(DBConnectionManager* manager, int m_id);
int Event1112AtMage12Up_Exists(DBConnectionManager* manager, int m_id);
int Event1112AtMage12Up_GetAll(DBConnectionManager* manager, Event1112AtMage12Up* records, int max_count, int* actual_count);

#endif /* EVENT_1112_AT_MAGE_12UP_ORM_H */