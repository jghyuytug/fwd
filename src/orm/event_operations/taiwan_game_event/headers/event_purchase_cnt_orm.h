#ifndef EVENT_PURCHASE_CNT_ORM_H
#define EVENT_PURCHASE_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int purchase_cnt;
} EventPurchaseCnt;

/* CRUD Operations */
int EventPurchaseCnt_Add(DBConnectionManager* manager, const EventPurchaseCnt* record);
int EventPurchaseCnt_Get(DBConnectionManager* manager, int m_id, EventPurchaseCnt* record);
int EventPurchaseCnt_Update(DBConnectionManager* manager, const EventPurchaseCnt* record);
int EventPurchaseCnt_Delete(DBConnectionManager* manager, int m_id);
int EventPurchaseCnt_Exists(DBConnectionManager* manager, int m_id);
int EventPurchaseCnt_GetAll(DBConnectionManager* manager, EventPurchaseCnt* records, int max_count, int* actual_count);

#endif /* EVENT_PURCHASE_CNT_ORM_H */