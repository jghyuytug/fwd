#ifndef DNF_ZEROMONEY_LOG_ORM_H
#define DNF_ZEROMONEY_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int to_m_id;
    char to_nat_ip[17];
    char to_peer_ip[17];
    signed char to_nat_type;
    char to_mac_addr[65];
    int from_m_id;
    char from_nat_ip[17];
    char from_peer_ip[17];
    signed char from_nat_type;
    char from_mac_addr[65];
    char occ_time[20];
    int injury_money;
} DnfZeromoneyLog;

/* CRUD Operations */
int DnfZeromoneyLog_Add(DBConnectionManager* manager, const DnfZeromoneyLog* record);
int DnfZeromoneyLog_GetAll(DBConnectionManager* manager, DnfZeromoneyLog* records, int max_count, int* actual_count);

#endif /* DNF_ZEROMONEY_LOG_ORM_H */