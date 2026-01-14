#ifndef LOG_CLIENT_TING_ORM_H
#define LOG_CLIENT_TING_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int channel_no;
    int reason;
    int cnt;
} LogClientTing;

/* CRUD Operations */
int LogClientTing_Add(DBConnectionManager* manager, const LogClientTing* record);
int LogClientTing_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason, LogClientTing* record);
int LogClientTing_Update(DBConnectionManager* manager, const LogClientTing* record);
int LogClientTing_Delete(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason);
int LogClientTing_Exists(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason);
int LogClientTing_GetAll(DBConnectionManager* manager, LogClientTing* records, int max_count, int* actual_count);

#endif /* LOG_CLIENT_TING_ORM_H */