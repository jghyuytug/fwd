#ifndef RELIABLE_MEMBER_LOG_ORM_H
#define RELIABLE_MEMBER_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int m_id;
    char ip[16];
} ReliableMemberLog;

/* CRUD Operations */
int ReliableMemberLog_Add(DBConnectionManager* manager, const ReliableMemberLog* record);
int ReliableMemberLog_Get(DBConnectionManager* manager, const char* occ_date, int m_id, const char* ip, ReliableMemberLog* record);
int ReliableMemberLog_Update(DBConnectionManager* manager, const ReliableMemberLog* record);
int ReliableMemberLog_Delete(DBConnectionManager* manager, const char* occ_date, int m_id, const char* ip);
int ReliableMemberLog_Exists(DBConnectionManager* manager, const char* occ_date, int m_id, const char* ip);
int ReliableMemberLog_GetAll(DBConnectionManager* manager, ReliableMemberLog* records, int max_count, int* actual_count);

#endif /* RELIABLE_MEMBER_LOG_ORM_H */