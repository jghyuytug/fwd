#ifndef EVENT_GOLDCARD_INFO_ORM_H
#define EVENT_GOLDCARD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned short coupon;
} EventGoldcardInfo;

/* CRUD Operations */
int EventGoldcardInfo_Add(DBConnectionManager* manager, const EventGoldcardInfo* info);
int EventGoldcardInfo_Get(DBConnectionManager* manager, int m_id, EventGoldcardInfo* info);
int EventGoldcardInfo_Update(DBConnectionManager* manager, const EventGoldcardInfo* info);
int EventGoldcardInfo_Delete(DBConnectionManager* manager, int m_id);
int EventGoldcardInfo_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int EventGoldcardInfo_GetAll(DBConnectionManager* manager, EventGoldcardInfo* infos,
                              int max_count, int* actual_count);
int EventGoldcardInfo_AddCoupon(DBConnectionManager* manager, int m_id, unsigned short amount);
int EventGoldcardInfo_UseCoupon(DBConnectionManager* manager, int m_id, unsigned short amount);
int EventGoldcardInfo_CountByCouponRange(DBConnectionManager* manager,
                                          unsigned short min_coupon, unsigned short max_coupon,
                                          int* count);

/* Utility */
void EventGoldcardInfo_PrintInfo(const EventGoldcardInfo* info);

#endif
