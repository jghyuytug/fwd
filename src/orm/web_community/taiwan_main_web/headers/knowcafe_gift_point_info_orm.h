#ifndef KNOWCAFE_GIFT_POINT_INFO_ORM_H
#define KNOWCAFE_GIFT_POINT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int lev;
    int point;
    int item_no;
    short item_cnt;
    short item_period;
} KnowcafeGiftPointInfo;

/* CRUD Operations */
int KnowcafeGiftPointInfo_Add(DBConnectionManager* manager, const KnowcafeGiftPointInfo* record);
int KnowcafeGiftPointInfo_GetAll(DBConnectionManager* manager, KnowcafeGiftPointInfo* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_GIFT_POINT_INFO_ORM_H */