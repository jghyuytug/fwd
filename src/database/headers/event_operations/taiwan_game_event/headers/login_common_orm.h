#ifndef LOGIN_COMMON_ORM_H
#define LOGIN_COMMON_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int update_time;
    int member_bonus_fatigue;
    signed char radio_flag;
    int daily_point;
    int acc_point;
    signed char random_option_guide;
    signed char event_charac_cnt;
    char login_ip[16];
    char inform_notice[256];
} LoginCommon;

/* CRUD Operations */
int LoginCommon_Add(DBConnectionManager* manager, const LoginCommon* record);
int LoginCommon_Get(DBConnectionManager* manager, int m_id, LoginCommon* record);
int LoginCommon_Update(DBConnectionManager* manager, const LoginCommon* record);
int LoginCommon_Delete(DBConnectionManager* manager, int m_id);
int LoginCommon_Exists(DBConnectionManager* manager, int m_id);
int LoginCommon_GetAll(DBConnectionManager* manager, LoginCommon* records, int max_count, int* actual_count);

#endif /* LOGIN_COMMON_ORM_H */