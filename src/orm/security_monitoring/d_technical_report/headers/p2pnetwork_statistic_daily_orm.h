#ifndef P2PNETWORK_STATISTIC_DAILY_ORM_H
#define P2PNETWORK_STATISTIC_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    float success_party;
    float dungeon_bad;
    float pvp_bad;
    float success_dungeon_clear;
    float fair_pvp_bad;
} P2PnetworkStatisticDaily;

/* CRUD Operations */
int P2PnetworkStatisticDaily_Add(DBConnectionManager* manager, const P2PnetworkStatisticDaily* record);
int P2PnetworkStatisticDaily_Get(DBConnectionManager* manager, const char* cur_date, P2PnetworkStatisticDaily* record);
int P2PnetworkStatisticDaily_Update(DBConnectionManager* manager, const P2PnetworkStatisticDaily* record);
int P2PnetworkStatisticDaily_Delete(DBConnectionManager* manager, const char* cur_date);
int P2PnetworkStatisticDaily_Exists(DBConnectionManager* manager, const char* cur_date);
int P2PnetworkStatisticDaily_GetAll(DBConnectionManager* manager, P2PnetworkStatisticDaily* records, int max_count, int* actual_count);

#endif /* P2PNETWORK_STATISTIC_DAILY_ORM_H */