#ifndef IP_MONITOR_PUNISH_ORM_H
#define IP_MONITOR_PUNISH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char ip[16];
    signed char type;
    unsigned short m_id_cnt;
    char start_time[20];
    char end_time[20];
} IpMonitorPunish;

/* CRUD Operations */
int IpMonitorPunish_Add(DBConnectionManager* manager, const IpMonitorPunish* punish);
int IpMonitorPunish_Get(DBConnectionManager* manager, const char* ip, signed char type, IpMonitorPunish* punish);
int IpMonitorPunish_Update(DBConnectionManager* manager, const IpMonitorPunish* punish);
int IpMonitorPunish_Delete(DBConnectionManager* manager, const char* ip, signed char type);
int IpMonitorPunish_Exists(DBConnectionManager* manager, const char* ip, signed char type);

/* Business Operations */
int IpMonitorPunish_GetAll(DBConnectionManager* manager,
                            IpMonitorPunish* punishes,
                            int max_count,
                            int* actual_count);
int IpMonitorPunish_GetByIp(DBConnectionManager* manager,
                             const char* ip,
                             IpMonitorPunish* punishes,
                             int max_count,
                             int* actual_count);
int IpMonitorPunish_GetByType(DBConnectionManager* manager,
                               signed char type,
                               IpMonitorPunish* punishes,
                               int max_count,
                               int* actual_count);
int IpMonitorPunish_DeleteExpired(DBConnectionManager* manager, const char* current_time);

/* Utility */
void IpMonitorPunish_PrintInfo(const IpMonitorPunish* punish);

#endif
