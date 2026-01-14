#ifndef DNF_EVENT_PRIZE_ORM_H
#define DNF_EVENT_PRIZE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int prize_id;
    int m_id;
    int check_time;
} DnfEventPrize;

/* CRUD Operations */
int DnfEventPrize_Add(DBConnectionManager* manager, const DnfEventPrize* record);
int DnfEventPrize_Get(DBConnectionManager* manager, int prize_id, int m_id, DnfEventPrize* record);
int DnfEventPrize_Update(DBConnectionManager* manager, const DnfEventPrize* record);
int DnfEventPrize_Delete(DBConnectionManager* manager, int prize_id, int m_id);
int DnfEventPrize_Exists(DBConnectionManager* manager, int prize_id, int m_id);
int DnfEventPrize_GetAll(DBConnectionManager* manager, DnfEventPrize* records, int max_count, int* actual_count);

#endif /* DNF_EVENT_PRIZE_ORM_H */