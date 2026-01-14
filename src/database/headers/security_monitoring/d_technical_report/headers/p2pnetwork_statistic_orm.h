#ifndef P2PNETWORK_STATISTIC_ORM_H
#define P2PNETWORK_STATISTIC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char occ_time[20];
    signed char server_group;
    int success_party_try;
    int total_party_try;
    int dungeon_bad_ping;
    int dungeon_total;
    int pvp_bad_ping;
    int pvp_total;
    int success_dungeon_clear;
    int total_dungeon_clear;
    int fair_pvp_total;
    int fair_pvp_bad_ping;
} P2PnetworkStatistic;

/* CRUD Operations */
int P2PnetworkStatistic_Add(DBConnectionManager* manager, const P2PnetworkStatistic* record);
int P2PnetworkStatistic_Get(DBConnectionManager* manager, int id, P2PnetworkStatistic* record);
int P2PnetworkStatistic_Update(DBConnectionManager* manager, const P2PnetworkStatistic* record);
int P2PnetworkStatistic_Delete(DBConnectionManager* manager, int id);
int P2PnetworkStatistic_Exists(DBConnectionManager* manager, int id);
int P2PnetworkStatistic_GetAll(DBConnectionManager* manager, P2PnetworkStatistic* records, int max_count, int* actual_count);

#endif /* P2PNETWORK_STATISTIC_ORM_H */