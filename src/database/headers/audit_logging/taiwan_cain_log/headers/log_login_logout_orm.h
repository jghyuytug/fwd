#ifndef LOG_LOGIN_LOGOUT_ORM_H
#define LOG_LOGIN_LOGOUT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char channel_no;
    signed char event_type;
    int count;
} LogLoginLogout;

/* CRUD Operations */
int LogLoginLogout_Add(DBConnectionManager* manager, const LogLoginLogout* record);
int LogLoginLogout_Get(DBConnectionManager* manager, const char* occ_time, signed char channel_no, signed char event_type, LogLoginLogout* record);
int LogLoginLogout_Update(DBConnectionManager* manager, const LogLoginLogout* record);
int LogLoginLogout_Delete(DBConnectionManager* manager, const char* occ_time, signed char channel_no, signed char event_type);
int LogLoginLogout_Exists(DBConnectionManager* manager, const char* occ_time, signed char channel_no, signed char event_type);
int LogLoginLogout_GetAll(DBConnectionManager* manager, LogLoginLogout* records, int max_count, int* actual_count);

#endif /* LOG_LOGIN_LOGOUT_ORM_H */