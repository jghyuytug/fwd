#ifndef DNF_PARTY_PING_HISTORY_ORM_H
#define DNF_PARTY_PING_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char str_ip[17];
    int m_id;
    char ip_1[17];
    signed char nat_type_1;
    int ping1;
    int mtu_1;
    char ip_2[17];
    signed char nat_type_2;
    int ping2;
    int mtu_2;
    char ip_3[17];
    signed char nat_type_3;
    int ping3;
    int mtu_3;
    char ip_4[17];
    signed char nat_type_4;
    int ping4;
    int mtu_4;
    char occ_time[20];
    int map_id;
    int dungeon_id;
    char end_reason[2];
} DnfPartyPingHistory;

/* CRUD Operations */
int DnfPartyPingHistory_Add(DBConnectionManager* manager, const DnfPartyPingHistory* record);
int DnfPartyPingHistory_Get(DBConnectionManager* manager, int id, DnfPartyPingHistory* record);
int DnfPartyPingHistory_Update(DBConnectionManager* manager, const DnfPartyPingHistory* record);
int DnfPartyPingHistory_Delete(DBConnectionManager* manager, int id);
int DnfPartyPingHistory_Exists(DBConnectionManager* manager, int id);
int DnfPartyPingHistory_GetAll(DBConnectionManager* manager, DnfPartyPingHistory* records, int max_count, int* actual_count);

#endif /* DNF_PARTY_PING_HISTORY_ORM_H */