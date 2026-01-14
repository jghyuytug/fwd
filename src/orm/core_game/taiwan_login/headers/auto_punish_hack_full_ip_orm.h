#ifndef AUTO_PUNISH_HACK_FULL_IP_ORM_H
#define AUTO_PUNISH_HACK_FULL_IP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];              /* date */
    unsigned short hack_type;       /* smallint(5) unsigned */
    unsigned short hack_sub_type;   /* smallint(5) unsigned */
    char full_ip[16];               /* varchar(15) */
    unsigned int cnt;               /* int(10) unsigned */
} AutoPunishHackFullIp;

/* CRUD Operations */
int AutoPunishHackFullIp_Add(DBConnectionManager* manager, const AutoPunishHackFullIp* info);
int AutoPunishHackFullIp_Get(DBConnectionManager* manager, const char* occ_date,
                              unsigned short hack_type, unsigned short hack_sub_type,
                              const char* full_ip, AutoPunishHackFullIp* info);
int AutoPunishHackFullIp_Update(DBConnectionManager* manager, const AutoPunishHackFullIp* info);
int AutoPunishHackFullIp_Delete(DBConnectionManager* manager, const char* occ_date,
                                 unsigned short hack_type, unsigned short hack_sub_type,
                                 const char* full_ip);
int AutoPunishHackFullIp_Exists(DBConnectionManager* manager, const char* occ_date,
                                 unsigned short hack_type, unsigned short hack_sub_type,
                                 const char* full_ip);

/* Business Operations */
int AutoPunishHackFullIp_GetAll(DBConnectionManager* manager, AutoPunishHackFullIp* infos,
                                 int max_count, int* actual_count);
int AutoPunishHackFullIp_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                    AutoPunishHackFullIp* infos, int max_count, int* actual_count);
int AutoPunishHackFullIp_GetByHackType(DBConnectionManager* manager, unsigned short hack_type,
                                        AutoPunishHackFullIp* infos, int max_count, int* actual_count);

/* Utility */
void AutoPunishHackFullIp_PrintInfo(const AutoPunishHackFullIp* info);

#endif
