#ifndef DNF_PARTY_STAT_HISTORY_ORM_H
#define DNF_PARTY_STAT_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char str_ip[17];
    int m_id;
    int try_num;
    char party_result[31];
    int party_num;
    int ping1;
    int ping2;
    int ping3;
    int ping4;
    char occ_time[20];
} DnfPartyStatHistory;

/* CRUD Operations */
int DnfPartyStatHistory_Add(DBConnectionManager* manager, const DnfPartyStatHistory* record);
int DnfPartyStatHistory_Get(DBConnectionManager* manager, int id, DnfPartyStatHistory* record);
int DnfPartyStatHistory_Update(DBConnectionManager* manager, const DnfPartyStatHistory* record);
int DnfPartyStatHistory_Delete(DBConnectionManager* manager, int id);
int DnfPartyStatHistory_Exists(DBConnectionManager* manager, int id);
int DnfPartyStatHistory_GetAll(DBConnectionManager* manager, DnfPartyStatHistory* records, int max_count, int* actual_count);

#endif /* DNF_PARTY_STAT_HISTORY_ORM_H */