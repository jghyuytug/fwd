#ifndef P2P_STATISTICS_ORM_H
#define P2P_STATISTICS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_group;
    int p2p_user;
    int p2p_min_ping;
    int p2p_max_ping;
    int p2p_avg_ping;
    int p2p_over_ping_100;
    int p2p_over_ping_200;
    int p2p_over_ping_300;
    int p2p_over_ping_400;
    int relay_user;
    int relay_min_ping;
    int relay_max_ping;
    int relay_avg_ping;
    int relay_over_ping_100;
    int relay_over_ping_200;
    int relay_over_ping_300;
    int relay_over_ping_400;
} P2PStatistics;

/* CRUD Operations */
int P2PStatistics_Add(DBConnectionManager* manager, const P2PStatistics* record);
int P2PStatistics_Get(DBConnectionManager* manager, const char* occ_time, signed char server_group, P2PStatistics* record);
int P2PStatistics_Update(DBConnectionManager* manager, const P2PStatistics* record);
int P2PStatistics_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_group);
int P2PStatistics_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_group);
int P2PStatistics_GetAll(DBConnectionManager* manager, P2PStatistics* records, int max_count, int* actual_count);

#endif /* P2P_STATISTICS_ORM_H */