#ifndef EVENT_WEBMONEYSTAMP_ITEM_ORM_H
#define EVENT_WEBMONEYSTAMP_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;                /* Unix timestamp */
    unsigned char server_id;
    unsigned char charac_no;
    unsigned int item_no;
    unsigned int item_check;
} EventWebmoneystampItem;

/* CRUD operations - composite primary key (m_id, occ_time) */
int EventWebmoneystampItem_Add(DBConnectionManager* manager, const EventWebmoneystampItem* item);
int EventWebmoneystampItem_Get(DBConnectionManager* manager, int m_id, int occ_time,
                                EventWebmoneystampItem* item);
int EventWebmoneystampItem_Update(DBConnectionManager* manager, const EventWebmoneystampItem* item);
int EventWebmoneystampItem_Delete(DBConnectionManager* manager, int m_id, int occ_time);
int EventWebmoneystampItem_Exists(DBConnectionManager* manager, int m_id, int occ_time);

/* Business operations */
int EventWebmoneystampItem_GetByMember(DBConnectionManager* manager, int m_id,
                                        EventWebmoneystampItem* items,
                                        int max_count, int* actual_count);
int EventWebmoneystampItem_GetByCharac(DBConnectionManager* manager, unsigned char server_id,
                                        unsigned char charac_no, EventWebmoneystampItem* items,
                                        int max_count, int* actual_count);
int EventWebmoneystampItem_GetByItemNo(DBConnectionManager* manager, unsigned int item_no,
                                        EventWebmoneystampItem* items,
                                        int max_count, int* actual_count);
int EventWebmoneystampItem_DeleteByMember(DBConnectionManager* manager, int m_id);

/* Utility */
void EventWebmoneystampItem_PrintInfo(const EventWebmoneystampItem* item);

#endif
