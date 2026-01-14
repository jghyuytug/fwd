#ifndef EVENT_WEBMONEYSTAMP_ITEM_ORM_H
#define EVENT_WEBMONEYSTAMP_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;
    signed char server_id;
    signed char charac_no;
    int item_no;
    int item_check;
} EventWebmoneystampItem;

/* CRUD Operations */
int EventWebmoneystampItem_Add(DBConnectionManager* manager, const EventWebmoneystampItem* record);
int EventWebmoneystampItem_Get(DBConnectionManager* manager, int m_id, int occ_time, EventWebmoneystampItem* record);
int EventWebmoneystampItem_Update(DBConnectionManager* manager, const EventWebmoneystampItem* record);
int EventWebmoneystampItem_Delete(DBConnectionManager* manager, int m_id, int occ_time);
int EventWebmoneystampItem_Exists(DBConnectionManager* manager, int m_id, int occ_time);
int EventWebmoneystampItem_GetAll(DBConnectionManager* manager, EventWebmoneystampItem* records, int max_count, int* actual_count);

#endif /* EVENT_WEBMONEYSTAMP_ITEM_ORM_H */