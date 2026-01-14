#ifndef AUTO_PUNISH_SECOND_USER_ORM_H
#define AUTO_PUNISH_SECOND_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;                           /* int(11) */
    char occ_time[20];                  /* datetime */
    unsigned int total_trade_cnt;       /* int(10) unsigned */
    unsigned int trade_cnt;             /* int(10) unsigned */
    unsigned long long total_trade_gold;/* bigint(20) unsigned */
    unsigned long long trade_gold;      /* bigint(20) unsigned */
    char punish_flag;                   /* tinyint(4) */
} AutoPunishSecondUser;

/* CRUD Operations */
int AutoPunishSecondUser_Add(DBConnectionManager* manager, const AutoPunishSecondUser* info);
int AutoPunishSecondUser_Get(DBConnectionManager* manager, int m_id, AutoPunishSecondUser* info);
int AutoPunishSecondUser_Update(DBConnectionManager* manager, const AutoPunishSecondUser* info);
int AutoPunishSecondUser_Delete(DBConnectionManager* manager, int m_id);
int AutoPunishSecondUser_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int AutoPunishSecondUser_GetAll(DBConnectionManager* manager, AutoPunishSecondUser* infos,
                                 int max_count, int* actual_count);
int AutoPunishSecondUser_GetByPunishFlag(DBConnectionManager* manager, char punish_flag,
                                          AutoPunishSecondUser* infos, int max_count, int* actual_count);
int AutoPunishSecondUser_GetTopTraders(DBConnectionManager* manager, AutoPunishSecondUser* infos,
                                        int max_count, int* actual_count);

/* Utility */
void AutoPunishSecondUser_PrintInfo(const AutoPunishSecondUser* info);

#endif
