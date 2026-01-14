#ifndef EVENT_GOLDCARD_CNT_ORM_H
#define EVENT_GOLDCARD_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int item_no;    /* Item number (primary key) */
    char occ_date[11];       /* Occurrence date YYYY-MM-DD (primary key) */
    int cnt;                 /* Count */
} EventGoldcardCnt;

/* CRUD operations */
int EventGoldcardCnt_Add(DBConnectionManager* manager, const EventGoldcardCnt* record);
int EventGoldcardCnt_Get(DBConnectionManager* manager, unsigned int item_no, const char* occ_date, EventGoldcardCnt* record);
int EventGoldcardCnt_Update(DBConnectionManager* manager, const EventGoldcardCnt* record);
int EventGoldcardCnt_Delete(DBConnectionManager* manager, unsigned int item_no, const char* occ_date);
int EventGoldcardCnt_Exists(DBConnectionManager* manager, unsigned int item_no, const char* occ_date);

/* Business operations */
int EventGoldcardCnt_GetByItemNo(DBConnectionManager* manager, unsigned int item_no,
                                  EventGoldcardCnt* records, int max_count, int* actual_count);
int EventGoldcardCnt_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                                     EventGoldcardCnt* records, int max_count, int* actual_count);
int EventGoldcardCnt_IncrementCount(DBConnectionManager* manager, unsigned int item_no, const char* occ_date, int increment);
int EventGoldcardCnt_GetTotalByItem(DBConnectionManager* manager, unsigned int item_no);
int EventGoldcardCnt_GetTotalByDate(DBConnectionManager* manager, const char* occ_date);

/* Utility */
void EventGoldcardCnt_PrintInfo(const EventGoldcardCnt* record);

#endif
