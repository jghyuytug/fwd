#ifndef AUTO_PUNISH_HACK_IP_ORM_H
#define AUTO_PUNISH_HACK_IP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    unsigned short hack_type;
    unsigned short hack_sub_type;
    char c_class_ip[13];
    unsigned int cnt;
} AutoPunishHackIp;

/* CRUD Operations */
int AutoPunishHackIp_Add(DBConnectionManager* manager, const AutoPunishHackIp* hack);
int AutoPunishHackIp_Get(DBConnectionManager* manager, const char* occ_date,
                          unsigned short hack_type, unsigned short hack_sub_type,
                          const char* c_class_ip, AutoPunishHackIp* hack);
int AutoPunishHackIp_Update(DBConnectionManager* manager, const AutoPunishHackIp* hack);
int AutoPunishHackIp_Delete(DBConnectionManager* manager, const char* occ_date,
                              unsigned short hack_type, unsigned short hack_sub_type,
                              const char* c_class_ip);
int AutoPunishHackIp_Exists(DBConnectionManager* manager, const char* occ_date,
                              unsigned short hack_type, unsigned short hack_sub_type,
                              const char* c_class_ip);

/* Business Operations */
int AutoPunishHackIp_GetAll(DBConnectionManager* manager, AutoPunishHackIp* hacks,
                             int max_count, int* actual_count);
int AutoPunishHackIp_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                AutoPunishHackIp* hacks, int max_count, int* actual_count);
int AutoPunishHackIp_GetByHackType(DBConnectionManager* manager, unsigned short hack_type,
                                    AutoPunishHackIp* hacks, int max_count, int* actual_count);

/* Utility Functions */
void AutoPunishHackIp_PrintInfo(const AutoPunishHackIp* hack);

#endif
