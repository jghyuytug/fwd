#ifndef EVENT_GOLDCARD_CNT_ORM_H
#define EVENT_GOLDCARD_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int item_no;
    char occ_date[11];
    int cnt;
} EventGoldcardCnt;

/* CRUD Operations */
int EventGoldcardCnt_Add(DBConnectionManager* manager, const EventGoldcardCnt* record);
int EventGoldcardCnt_Get(DBConnectionManager* manager, int item_no, const char* occ_date, EventGoldcardCnt* record);
int EventGoldcardCnt_Update(DBConnectionManager* manager, const EventGoldcardCnt* record);
int EventGoldcardCnt_Delete(DBConnectionManager* manager, int item_no, const char* occ_date);
int EventGoldcardCnt_Exists(DBConnectionManager* manager, int item_no, const char* occ_date);
int EventGoldcardCnt_GetAll(DBConnectionManager* manager, EventGoldcardCnt* records, int max_count, int* actual_count);

#endif /* EVENT_GOLDCARD_CNT_ORM_H */