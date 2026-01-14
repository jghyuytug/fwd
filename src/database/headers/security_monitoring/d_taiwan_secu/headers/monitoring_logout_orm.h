#ifndef MONITORING_LOGOUT_ORM_H
#define MONITORING_LOGOUT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int logout_time;
    int logout_ip;
    signed char otp_del_type;
} MonitoringLogout;

/* CRUD Operations */
int MonitoringLogout_Add(DBConnectionManager* manager, const MonitoringLogout* record);
int MonitoringLogout_Get(DBConnectionManager* manager, int no, MonitoringLogout* record);
int MonitoringLogout_Update(DBConnectionManager* manager, const MonitoringLogout* record);
int MonitoringLogout_Delete(DBConnectionManager* manager, int no);
int MonitoringLogout_Exists(DBConnectionManager* manager, int no);
int MonitoringLogout_GetAll(DBConnectionManager* manager, MonitoringLogout* records, int max_count, int* actual_count);

#endif /* MONITORING_LOGOUT_ORM_H */