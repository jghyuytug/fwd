#ifndef KNOWCAFE_GIFT_GIVE_STATS_ORM_H
#define KNOWCAFE_GIFT_GIVE_STATS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char date_enroll[11];
    int item_no;
    int item_cnt;
} KnowcafeGiftGiveStats;

/* CRUD Operations */
int KnowcafeGiftGiveStats_Add(DBConnectionManager* manager, const KnowcafeGiftGiveStats* record);
int KnowcafeGiftGiveStats_Get(DBConnectionManager* manager, const char* date_enroll, int item_no, KnowcafeGiftGiveStats* record);
int KnowcafeGiftGiveStats_Update(DBConnectionManager* manager, const KnowcafeGiftGiveStats* record);
int KnowcafeGiftGiveStats_Delete(DBConnectionManager* manager, const char* date_enroll, int item_no);
int KnowcafeGiftGiveStats_Exists(DBConnectionManager* manager, const char* date_enroll, int item_no);
int KnowcafeGiftGiveStats_GetAll(DBConnectionManager* manager, KnowcafeGiftGiveStats* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_GIFT_GIVE_STATS_ORM_H */