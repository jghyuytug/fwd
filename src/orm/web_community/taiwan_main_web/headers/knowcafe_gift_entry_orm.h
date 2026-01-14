#ifndef KNOWCAFE_GIFT_ENTRY_ORM_H
#define KNOWCAFE_GIFT_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int occ_date;
    signed char server_id;
    int charac_no;
    int item_no;
    short item_cnt;
    int item_check;
    int total_point;
    signed char occ_type;
} KnowcafeGiftEntry;

/* CRUD Operations */
int KnowcafeGiftEntry_Add(DBConnectionManager* manager, const KnowcafeGiftEntry* record);
int KnowcafeGiftEntry_Get(DBConnectionManager* manager, int no, KnowcafeGiftEntry* record);
int KnowcafeGiftEntry_Update(DBConnectionManager* manager, const KnowcafeGiftEntry* record);
int KnowcafeGiftEntry_Delete(DBConnectionManager* manager, int no);
int KnowcafeGiftEntry_Exists(DBConnectionManager* manager, int no);
int KnowcafeGiftEntry_GetAll(DBConnectionManager* manager, KnowcafeGiftEntry* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_GIFT_ENTRY_ORM_H */