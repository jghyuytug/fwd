#ifndef DNF_TOWER_MANAGER_ORM_H
#define DNF_TOWER_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int occ_time;
    char manager_id[13];
    signed char server_id;
    signed char web_flag;
    signed char game_falg;
} DnfTowerManager;

/* CRUD Operations */
int DnfTowerManager_Add(DBConnectionManager* manager, const DnfTowerManager* record);
int DnfTowerManager_GetAll(DBConnectionManager* manager, DnfTowerManager* records, int max_count, int* actual_count);

#endif /* DNF_TOWER_MANAGER_ORM_H */