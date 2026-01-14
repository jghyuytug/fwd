#ifndef DNF_PARTY_PING_HISTORY_PVP_ORM_H
#define DNF_PARTY_PING_HISTORY_PVP_ORM_H

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
    char ip_5[17];
    signed char nat_type_5;
    int ping5;
    int mtu_5;
    char ip_6[17];
    signed char nat_type_6;
    int ping6;
    int mtu_6;
    char ip_7[17];
    signed char nat_type_7;
    int ping7;
    int mtu_7;
    char ip_8[17];
    signed char nat_type_8;
    int ping8;
    int mtu_8;
    char occ_time[20];
    int map_id;
    int dungeon_id;
    char end_reason[2];
} DnfPartyPingHistoryPvp;

/* CRUD Operations */
int DnfPartyPingHistoryPvp_Add(DBConnectionManager* manager, const DnfPartyPingHistoryPvp* record);
int DnfPartyPingHistoryPvp_Get(DBConnectionManager* manager, int id, DnfPartyPingHistoryPvp* record);
int DnfPartyPingHistoryPvp_Update(DBConnectionManager* manager, const DnfPartyPingHistoryPvp* record);
int DnfPartyPingHistoryPvp_Delete(DBConnectionManager* manager, int id);
int DnfPartyPingHistoryPvp_Exists(DBConnectionManager* manager, int id);
int DnfPartyPingHistoryPvp_GetAll(DBConnectionManager* manager, DnfPartyPingHistoryPvp* records, int max_count, int* actual_count);

#endif /* DNF_PARTY_PING_HISTORY_PVP_ORM_H */