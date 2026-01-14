#ifndef POWER_WAR_STATUE_RANKER_ORM_H
#define POWER_WAR_STATUE_RANKER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    int first_ranker;
    int second_ranker;
    int third_ranker;
} PowerWarStatueRanker;

/* CRUD Operations */
int PowerWarStatueRanker_Add(DBConnectionManager* manager, const PowerWarStatueRanker* record);
int PowerWarStatueRanker_Get(DBConnectionManager* manager, signed char server_id, PowerWarStatueRanker* record);
int PowerWarStatueRanker_Update(DBConnectionManager* manager, const PowerWarStatueRanker* record);
int PowerWarStatueRanker_Delete(DBConnectionManager* manager, signed char server_id);
int PowerWarStatueRanker_Exists(DBConnectionManager* manager, signed char server_id);
int PowerWarStatueRanker_GetAll(DBConnectionManager* manager, PowerWarStatueRanker* records, int max_count, int* actual_count);

#endif /* POWER_WAR_STATUE_RANKER_ORM_H */