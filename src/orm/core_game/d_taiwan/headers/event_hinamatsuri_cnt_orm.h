#ifndef EVENT_HINAMATSURI_CNT_ORM_H
#define EVENT_HINAMATSURI_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int cnt;
} EventHinamatsuriCnt;

/* Global counter operations - single row table */
int EventHinamatsuriCnt_Get(DBConnectionManager* manager, int* cnt);
int EventHinamatsuriCnt_Set(DBConnectionManager* manager, int cnt);
int EventHinamatsuriCnt_Increment(DBConnectionManager* manager, int delta);
int EventHinamatsuriCnt_Reset(DBConnectionManager* manager);

/* Utility */
void EventHinamatsuriCnt_PrintInfo(const EventHinamatsuriCnt* counter);

#endif
