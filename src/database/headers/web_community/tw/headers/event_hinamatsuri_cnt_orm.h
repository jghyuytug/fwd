#ifndef EVENT_HINAMATSURI_CNT_ORM_H
#define EVENT_HINAMATSURI_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int cnt;
} EventHinamatsuriCnt;

/* CRUD Operations */
int EventHinamatsuriCnt_Add(DBConnectionManager* manager, const EventHinamatsuriCnt* record);
int EventHinamatsuriCnt_GetAll(DBConnectionManager* manager, EventHinamatsuriCnt* records, int max_count, int* actual_count);

#endif /* EVENT_HINAMATSURI_CNT_ORM_H */