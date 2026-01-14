#ifndef PASSWD_MOD_ENTRY_ORM_H
#define PASSWD_MOD_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    char ip[16];
    char pre_passwd[33];
} PasswdModEntry;

/* CRUD Operations - Note: Composite primary key (m_id, occ_time) */
int PasswdModEntry_Add(DBConnectionManager* manager, const PasswdModEntry* entry);
int PasswdModEntry_Get(DBConnectionManager* manager, int m_id, const char* occ_time,
                       PasswdModEntry* entry);
int PasswdModEntry_Update(DBConnectionManager* manager, int m_id, const char* occ_time,
                          const PasswdModEntry* entry);
int PasswdModEntry_Delete(DBConnectionManager* manager, int m_id, const char* occ_time);
int PasswdModEntry_Exists(DBConnectionManager* manager, int m_id, const char* occ_time);

/* Business Operations */
int PasswdModEntry_GetByMemberID(DBConnectionManager* manager, int m_id,
                                  PasswdModEntry* entries, int max_count, int* actual_count);
int PasswdModEntry_GetByIP(DBConnectionManager* manager, const char* ip,
                           PasswdModEntry* entries, int max_count, int* actual_count);
int PasswdModEntry_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                   const char* end_time, PasswdModEntry* entries,
                                   int max_count, int* actual_count);
int PasswdModEntry_GetRecent(DBConnectionManager* manager, PasswdModEntry* entries,
                              int max_count, int* actual_count);
int PasswdModEntry_DeleteByMemberID(DBConnectionManager* manager, int m_id);

/* Utility Functions */
void PasswdModEntry_PrintInfo(const PasswdModEntry* entry);

#endif
