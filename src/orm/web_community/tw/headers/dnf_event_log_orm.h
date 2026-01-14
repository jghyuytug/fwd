#ifndef DNF_EVENT_LOG_ORM_H
#define DNF_EVENT_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int log_id;
    int occ_time;
    signed char event_type;
    int parameter1;
    int parameter2;
    signed char server_id;
    signed char event_flag;
    int start_time;
    int end_time;
    int m_id;
    char expl[201];
    char etc[101];
} DnfEventLog;

/* CRUD Operations */
int DnfEventLog_Add(DBConnectionManager* manager, const DnfEventLog* record);
int DnfEventLog_Get(DBConnectionManager* manager, int log_id, DnfEventLog* record);
int DnfEventLog_Update(DBConnectionManager* manager, const DnfEventLog* record);
int DnfEventLog_Delete(DBConnectionManager* manager, int log_id);
int DnfEventLog_Exists(DBConnectionManager* manager, int log_id);
int DnfEventLog_GetAll(DBConnectionManager* manager, DnfEventLog* records, int max_count, int* actual_count);

#endif /* DNF_EVENT_LOG_ORM_H */