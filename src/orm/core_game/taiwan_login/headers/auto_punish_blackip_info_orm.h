#ifndef AUTO_PUNISH_BLACKIP_INFO_ORM_H
#define AUTO_PUNISH_BLACKIP_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char ip[12];                    /* varchar(11) */
    unsigned char start_ip;         /* tinyint(3) unsigned */
    unsigned char end_ip;           /* tinyint(3) unsigned */
    char reg_date[20];              /* datetime */
    char apply_flag;                /* tinyint(4) */
} AutoPunishBlackipInfo;

/* CRUD Operations */
int AutoPunishBlackipInfo_Add(DBConnectionManager* manager, const AutoPunishBlackipInfo* info);
int AutoPunishBlackipInfo_Get(DBConnectionManager* manager, const char* ip,
                                unsigned char start_ip, unsigned char end_ip,
                                AutoPunishBlackipInfo* info);
int AutoPunishBlackipInfo_Update(DBConnectionManager* manager, const AutoPunishBlackipInfo* info);
int AutoPunishBlackipInfo_Delete(DBConnectionManager* manager, const char* ip,
                                  unsigned char start_ip, unsigned char end_ip);
int AutoPunishBlackipInfo_Exists(DBConnectionManager* manager, const char* ip,
                                  unsigned char start_ip, unsigned char end_ip);

/* Business Operations */
int AutoPunishBlackipInfo_GetAll(DBConnectionManager* manager, AutoPunishBlackipInfo* infos,
                                  int max_count, int* actual_count);
int AutoPunishBlackipInfo_GetByApplyFlag(DBConnectionManager* manager, char apply_flag,
                                          AutoPunishBlackipInfo* infos,
                                          int max_count, int* actual_count);

/* Utility */
void AutoPunishBlackipInfo_PrintInfo(const AutoPunishBlackipInfo* info);

#endif
