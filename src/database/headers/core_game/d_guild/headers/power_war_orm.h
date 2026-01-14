#ifndef POWER_WAR_ORM_H
#define POWER_WAR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    int a_side_point;
    int b_side_point;
    signed char winner_side;
    char occ_time[20];
} PowerWar;

/* CRUD Operations */
int PowerWar_Add(DBConnectionManager* manager, const PowerWar* record);
int PowerWar_Get(DBConnectionManager* manager, signed char server_id, PowerWar* record);
int PowerWar_Update(DBConnectionManager* manager, const PowerWar* record);
int PowerWar_Delete(DBConnectionManager* manager, signed char server_id);
int PowerWar_Exists(DBConnectionManager* manager, signed char server_id);
int PowerWar_GetAll(DBConnectionManager* manager, PowerWar* records, int max_count, int* actual_count);

#endif /* POWER_WAR_ORM_H */