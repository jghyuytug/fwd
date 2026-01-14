#ifndef POWER_WAR_USER_RANK_ORM_H
#define POWER_WAR_USER_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    short rank;
    int charac_no;
    int power_war_point;
    signed char power_side;
} PowerWarUserRank;

/* CRUD Operations */
int PowerWarUserRank_Add(DBConnectionManager* manager, const PowerWarUserRank* record);
int PowerWarUserRank_Get(DBConnectionManager* manager, int charac_no, signed char server_id, PowerWarUserRank* record);
int PowerWarUserRank_Update(DBConnectionManager* manager, const PowerWarUserRank* record);
int PowerWarUserRank_Delete(DBConnectionManager* manager, int charac_no, signed char server_id);
int PowerWarUserRank_Exists(DBConnectionManager* manager, int charac_no, signed char server_id);
int PowerWarUserRank_GetAll(DBConnectionManager* manager, PowerWarUserRank* records, int max_count, int* actual_count);

#endif /* POWER_WAR_USER_RANK_ORM_H */