#ifndef LOGIN_VIEW_LOG_ORM_H
#define LOGIN_VIEW_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int m_id;
    int charac_m_id;
    int charac_no;
    char ip[16];
} LoginViewLog;

/* CRUD Operations */
int LoginViewLog_Add(DBConnectionManager* manager, const LoginViewLog* record);
int LoginViewLog_Get(DBConnectionManager* manager, const char* occ_time, int m_id, LoginViewLog* record);
int LoginViewLog_Update(DBConnectionManager* manager, const LoginViewLog* record);
int LoginViewLog_Delete(DBConnectionManager* manager, const char* occ_time, int m_id);
int LoginViewLog_Exists(DBConnectionManager* manager, const char* occ_time, int m_id);
int LoginViewLog_GetAll(DBConnectionManager* manager, LoginViewLog* records, int max_count, int* actual_count);

#endif /* LOGIN_VIEW_LOG_ORM_H */