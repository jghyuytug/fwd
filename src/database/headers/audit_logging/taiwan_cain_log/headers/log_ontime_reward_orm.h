#ifndef LOG_ONTIME_REWARD_ORM_H
#define LOG_ONTIME_REWARD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char time[20];
    int event_no;
    int reward_cnt;
} LogOntimeReward;

/* CRUD Operations */
int LogOntimeReward_Add(DBConnectionManager* manager, const LogOntimeReward* record);
int LogOntimeReward_Get(DBConnectionManager* manager, const char* time, int event_no, LogOntimeReward* record);
int LogOntimeReward_Update(DBConnectionManager* manager, const LogOntimeReward* record);
int LogOntimeReward_Delete(DBConnectionManager* manager, const char* time, int event_no);
int LogOntimeReward_Exists(DBConnectionManager* manager, const char* time, int event_no);
int LogOntimeReward_GetAll(DBConnectionManager* manager, LogOntimeReward* records, int max_count, int* actual_count);

#endif /* LOG_ONTIME_REWARD_ORM_H */