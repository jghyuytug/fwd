#ifndef GEN_EVENT_ORM_H
#define GEN_EVENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    char stt_date[11];
    char end_date[11];
    char ann_date[11];
    char page_url[101];
    signed char event_type;
    char search_word[1024];
    char result_url[101];
} GenEvent;

/* CRUD Operations */
int GenEvent_Add(DBConnectionManager* manager, const GenEvent* record);
int GenEvent_Get(DBConnectionManager* manager, int gno, GenEvent* record);
int GenEvent_Update(DBConnectionManager* manager, const GenEvent* record);
int GenEvent_Delete(DBConnectionManager* manager, int gno);
int GenEvent_Exists(DBConnectionManager* manager, int gno);
int GenEvent_GetAll(DBConnectionManager* manager, GenEvent* records, int max_count, int* actual_count);

#endif /* GEN_EVENT_ORM_H */