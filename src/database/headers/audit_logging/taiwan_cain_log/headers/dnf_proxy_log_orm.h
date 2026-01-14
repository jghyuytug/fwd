#ifndef DNF_PROXY_LOG_ORM_H
#define DNF_PROXY_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int m_id;
    char nat_ip[17];
    char peer_ip[17];
    signed char nat_tye;
} DnfProxyLog;

/* CRUD Operations */
int DnfProxyLog_Add(DBConnectionManager* manager, const DnfProxyLog* record);
int DnfProxyLog_Get(DBConnectionManager* manager, const char* occ_time, int m_id, DnfProxyLog* record);
int DnfProxyLog_Update(DBConnectionManager* manager, const DnfProxyLog* record);
int DnfProxyLog_Delete(DBConnectionManager* manager, const char* occ_time, int m_id);
int DnfProxyLog_Exists(DBConnectionManager* manager, const char* occ_time, int m_id);
int DnfProxyLog_GetAll(DBConnectionManager* manager, DnfProxyLog* records, int max_count, int* actual_count);

#endif /* DNF_PROXY_LOG_ORM_H */