#ifndef EVENT_GOLDCARD_INFO_ORM_H
#define EVENT_GOLDCARD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    short coupon;
} EventGoldcardInfo;

/* CRUD Operations */
int EventGoldcardInfo_Add(DBConnectionManager* manager, const EventGoldcardInfo* record);
int EventGoldcardInfo_Get(DBConnectionManager* manager, int m_id, EventGoldcardInfo* record);
int EventGoldcardInfo_Update(DBConnectionManager* manager, const EventGoldcardInfo* record);
int EventGoldcardInfo_Delete(DBConnectionManager* manager, int m_id);
int EventGoldcardInfo_Exists(DBConnectionManager* manager, int m_id);
int EventGoldcardInfo_GetAll(DBConnectionManager* manager, EventGoldcardInfo* records, int max_count, int* actual_count);

#endif /* EVENT_GOLDCARD_INFO_ORM_H */