#ifndef AUTO_PUNISH_SECOND_LOG_ORM_H
#define AUTO_PUNISH_SECOND_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;                       /* int(11) */
    int hack_m_id;                  /* int(11) */
    char occ_time[20];              /* datetime */
    unsigned int trade_cnt;         /* int(10) unsigned */
    unsigned long long trade_gold;  /* bigint(20) unsigned */
} AutoPunishSecondLog;

/* CRUD Operations */
int AutoPunishSecondLog_Add(DBConnectionManager* manager, const AutoPunishSecondLog* info);
int AutoPunishSecondLog_Get(DBConnectionManager* manager, int m_id, int hack_m_id,
                             AutoPunishSecondLog* info);
int AutoPunishSecondLog_Update(DBConnectionManager* manager, const AutoPunishSecondLog* info);
int AutoPunishSecondLog_Delete(DBConnectionManager* manager, int m_id, int hack_m_id);
int AutoPunishSecondLog_Exists(DBConnectionManager* manager, int m_id, int hack_m_id);

/* Business Operations */
int AutoPunishSecondLog_GetAll(DBConnectionManager* manager, AutoPunishSecondLog* infos,
                                int max_count, int* actual_count);
int AutoPunishSecondLog_GetByMemberId(DBConnectionManager* manager, int m_id,
                                       AutoPunishSecondLog* infos, int max_count, int* actual_count);
int AutoPunishSecondLog_GetByHackMemberId(DBConnectionManager* manager, int hack_m_id,
                                           AutoPunishSecondLog* infos, int max_count, int* actual_count);

/* Utility */
void AutoPunishSecondLog_PrintInfo(const AutoPunishSecondLog* info);

#endif
