#ifndef TCG_RELAY_ENTRY_COUNT_ORM_H
#define TCG_RELAY_ENTRY_COUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_seq;
    int coupon_count;
} TcgRelayEntryCount;

/* CRUD Operations */
int TcgRelayEntryCount_Add(DBConnectionManager* manager, const TcgRelayEntryCount* record);
int TcgRelayEntryCount_Get(DBConnectionManager* manager, int event_seq, TcgRelayEntryCount* record);
int TcgRelayEntryCount_Update(DBConnectionManager* manager, const TcgRelayEntryCount* record);
int TcgRelayEntryCount_Delete(DBConnectionManager* manager, int event_seq);
int TcgRelayEntryCount_Exists(DBConnectionManager* manager, int event_seq);
int TcgRelayEntryCount_GetAll(DBConnectionManager* manager, TcgRelayEntryCount* records, int max_count, int* actual_count);

#endif /* TCG_RELAY_ENTRY_COUNT_ORM_H */