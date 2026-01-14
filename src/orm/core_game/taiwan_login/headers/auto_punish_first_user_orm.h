#ifndef AUTO_PUNISH_FIRST_USER_ORM_H
#define AUTO_PUNISH_FIRST_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    char ip[16];
    unsigned short hack_type;
    unsigned int cnt;
    char punish_flag;
    unsigned short hack_sub_type;
    unsigned int hack_sub_cnt;
} AutoPunishFirstUser;

int AutoPunishFirstUser_Add(DBConnectionManager* manager, const AutoPunishFirstUser* user);
int AutoPunishFirstUser_Get(DBConnectionManager* manager, int m_id,
                             unsigned short hack_type, unsigned short hack_sub_type,
                             AutoPunishFirstUser* user);
int AutoPunishFirstUser_Update(DBConnectionManager* manager, const AutoPunishFirstUser* user);
int AutoPunishFirstUser_Delete(DBConnectionManager* manager, int m_id,
                                unsigned short hack_type, unsigned short hack_sub_type);
int AutoPunishFirstUser_Exists(DBConnectionManager* manager, int m_id,
                                unsigned short hack_type, unsigned short hack_sub_type);
int AutoPunishFirstUser_GetByMId(DBConnectionManager* manager, int m_id,
                                  AutoPunishFirstUser* users, int max_count, int* actual_count);
int AutoPunishFirstUser_GetByPunishFlag(DBConnectionManager* manager, char punish_flag,
                                         AutoPunishFirstUser* users, int max_count, int* actual_count);
void AutoPunishFirstUser_PrintInfo(const AutoPunishFirstUser* user);

#endif
