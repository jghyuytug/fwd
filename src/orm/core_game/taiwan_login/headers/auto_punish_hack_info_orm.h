#ifndef AUTO_PUNISH_HACK_INFO_ORM_H
#define AUTO_PUNISH_HACK_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned short hack_type;       /* smallint(5) unsigned */
    unsigned int cnt;               /* int(10) unsigned */
    unsigned long long etc;         /* bigint(20) unsigned */
    char reg_date[20];              /* datetime */
    char apply_flag;                /* tinyint(4) */
    unsigned short hack_sub_type;   /* smallint(5) unsigned */
    unsigned int hack_sub_cnt;      /* int(10) unsigned */
    unsigned int ip_cnt;            /* int(10) unsigned */
} AutoPunishHackInfo;

/* CRUD Operations */
int AutoPunishHackInfo_Add(DBConnectionManager* manager, const AutoPunishHackInfo* info);
int AutoPunishHackInfo_Get(DBConnectionManager* manager, unsigned short hack_type,
                            char apply_flag, unsigned short hack_sub_type,
                            AutoPunishHackInfo* info);
int AutoPunishHackInfo_Update(DBConnectionManager* manager, const AutoPunishHackInfo* info);
int AutoPunishHackInfo_Delete(DBConnectionManager* manager, unsigned short hack_type,
                               char apply_flag, unsigned short hack_sub_type);
int AutoPunishHackInfo_Exists(DBConnectionManager* manager, unsigned short hack_type,
                               char apply_flag, unsigned short hack_sub_type);

/* Business Operations */
int AutoPunishHackInfo_GetAll(DBConnectionManager* manager, AutoPunishHackInfo* infos,
                               int max_count, int* actual_count);
int AutoPunishHackInfo_GetByHackType(DBConnectionManager* manager, unsigned short hack_type,
                                      AutoPunishHackInfo* infos, int max_count, int* actual_count);
int AutoPunishHackInfo_GetByApplyFlag(DBConnectionManager* manager, char apply_flag,
                                       AutoPunishHackInfo* infos, int max_count, int* actual_count);

/* Utility */
void AutoPunishHackInfo_PrintInfo(const AutoPunishHackInfo* info);

#endif
