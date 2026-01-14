#ifndef DNF_PARTY_RESULT_HISTORY_PVP_ORM_H
#define DNF_PARTY_RESULT_HISTORY_PVP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char str_ip[17];
    signed char nat_type;
    int m_id;
    signed char party_result;
    char occ_time[20];
    int map_id;
    int dungeon_id;
    char peer_ip[17];
    signed char peer_nat_type;
    int established_time;
    char IsConnectedToRelaySrv[2];
    int ConnectedTimeToRelaySrv;
    char relay_ip[17];
    int relay_udp_port;
    int relay_tcp_port;
} DnfPartyResultHistoryPvp;

/* CRUD Operations */
int DnfPartyResultHistoryPvp_Add(DBConnectionManager* manager, const DnfPartyResultHistoryPvp* record);
int DnfPartyResultHistoryPvp_Get(DBConnectionManager* manager, int id, DnfPartyResultHistoryPvp* record);
int DnfPartyResultHistoryPvp_Update(DBConnectionManager* manager, const DnfPartyResultHistoryPvp* record);
int DnfPartyResultHistoryPvp_Delete(DBConnectionManager* manager, int id);
int DnfPartyResultHistoryPvp_Exists(DBConnectionManager* manager, int id);
int DnfPartyResultHistoryPvp_GetAll(DBConnectionManager* manager, DnfPartyResultHistoryPvp* records, int max_count, int* actual_count);

#endif /* DNF_PARTY_RESULT_HISTORY_PVP_ORM_H */