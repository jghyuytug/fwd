#ifndef MEMBER_LOGIN_BACKUP_ORM_H
#define MEMBER_LOGIN_BACKUP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned int login_time;
    unsigned int expire_time;
    unsigned int last_play_time;
    unsigned int total_account_fail;
    char account_fail;
    int report_cnt;
    char reliable_flag;
    unsigned int trade_gold_daily;
    unsigned int last_gift_time;
    unsigned short gift_cnt;
    char login_ip[16];
    char security_flag;
    char power_side;
    unsigned int dungeon_gain_gold;
    int school_id;
    float rating;
    unsigned int cleanpad_point;
    char tutorial_skipable;
} MemberLoginBackup;

/* CRUD Operations */
int MemberLoginBackup_Add(DBConnectionManager* manager, const MemberLoginBackup* backup);
int MemberLoginBackup_GetByMemberId(DBConnectionManager* manager, int m_id,
                                     MemberLoginBackup* backups, int max_count, int* actual_count);
int MemberLoginBackup_DeleteByMemberId(DBConnectionManager* manager, int m_id);
int MemberLoginBackup_DeleteAll(DBConnectionManager* manager);

/* Business Operations */
int MemberLoginBackup_GetByIp(DBConnectionManager* manager, const char* login_ip,
                               MemberLoginBackup* backups, int max_count, int* actual_count);
int MemberLoginBackup_GetByDateRange(DBConnectionManager* manager,
                                       unsigned int start_time, unsigned int end_time,
                                       MemberLoginBackup* backups, int max_count, int* actual_count);
int MemberLoginBackup_CountByMemberId(DBConnectionManager* manager, int m_id);

/* Utility Functions */
void MemberLoginBackup_PrintInfo(const MemberLoginBackup* backup);

#endif
