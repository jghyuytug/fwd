#ifndef MEMBER_ENV_LOG_ORM_H
#define MEMBER_ENV_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char mac_addr[65];
    char log[1024];
    char occ_time[20];
} MemberEnvLog;

/* CRUD Operations */
int MemberEnvLog_Add(DBConnectionManager* manager, const MemberEnvLog* record);
int MemberEnvLog_GetAll(DBConnectionManager* manager, MemberEnvLog* records, int max_count, int* actual_count);

#endif /* MEMBER_ENV_LOG_ORM_H */