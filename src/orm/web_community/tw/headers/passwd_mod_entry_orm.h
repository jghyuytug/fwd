#ifndef PASSWD_MOD_ENTRY_ORM_H
#define PASSWD_MOD_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    char ip[16];
    char pre_passwd[33];
} PasswdModEntry;

/* CRUD Operations */
int PasswdModEntry_Add(DBConnectionManager* manager, const PasswdModEntry* record);
int PasswdModEntry_Get(DBConnectionManager* manager, int m_id, const char* occ_time, PasswdModEntry* record);
int PasswdModEntry_Update(DBConnectionManager* manager, const PasswdModEntry* record);
int PasswdModEntry_Delete(DBConnectionManager* manager, int m_id, const char* occ_time);
int PasswdModEntry_Exists(DBConnectionManager* manager, int m_id, const char* occ_time);
int PasswdModEntry_GetAll(DBConnectionManager* manager, PasswdModEntry* records, int max_count, int* actual_count);

#endif /* PASSWD_MOD_ENTRY_ORM_H */