#ifndef KNOWCAFE_GIFT_RANK_INFO_ORM_H
#define KNOWCAFE_GIFT_RANK_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rank;
    int item_no;
    short item_cnt;
    short item_period;
} KnowcafeGiftRankInfo;

/* CRUD Operations */
int KnowcafeGiftRankInfo_Add(DBConnectionManager* manager, const KnowcafeGiftRankInfo* record);
int KnowcafeGiftRankInfo_GetAll(DBConnectionManager* manager, KnowcafeGiftRankInfo* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_GIFT_RANK_INFO_ORM_H */