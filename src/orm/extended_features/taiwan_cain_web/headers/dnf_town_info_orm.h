#ifndef DNF_TOWN_INFO_ORM_H
#define DNF_TOWN_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    char town_name_kr[121];
} DnfTownInfo;

/* CRUD Operations */
int DnfTownInfo_Add(DBConnectionManager* manager, const DnfTownInfo* record);
int DnfTownInfo_GetAll(DBConnectionManager* manager, DnfTownInfo* records, int max_count, int* actual_count);

#endif /* DNF_TOWN_INFO_ORM_H */