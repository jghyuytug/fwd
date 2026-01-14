#ifndef POWER_WAR_STATUE_RANKER_H
#define POWER_WAR_STATUE_RANKER_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_id;
    unsigned int first_ranker;
    unsigned int second_ranker;
    unsigned int third_ranker;
} PowerWarStatueRanker;

// CRUD Operations
int PowerWarStatueRanker_Add(DBConnectionManager* manager, const PowerWarStatueRanker* record);
int PowerWarStatueRanker_Get(DBConnectionManager* manager, unsigned char server_id, PowerWarStatueRanker* out_record);
int PowerWarStatueRanker_Update(DBConnectionManager* manager, const PowerWarStatueRanker* record);
int PowerWarStatueRanker_Delete(DBConnectionManager* manager, unsigned char server_id);
int PowerWarStatueRanker_Exists(DBConnectionManager* manager, unsigned char server_id);

// Business Logic
int PowerWarStatueRanker_GetAllServers(DBConnectionManager* manager, PowerWarStatueRanker** out_records, int* out_count);
int PowerWarStatueRanker_UpdateRanker(DBConnectionManager* manager, unsigned char server_id, int position, unsigned int charac_no);
int PowerWarStatueRanker_GetRankerPosition(DBConnectionManager* manager, unsigned int charac_no, unsigned char* out_server_id, int* out_position);

#endif // POWER_WAR_STATUE_RANKER_H
