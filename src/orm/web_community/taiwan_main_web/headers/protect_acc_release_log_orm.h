#ifndef PROTECT_ACC_RELEASE_LOG_ORM_H
#define PROTECT_ACC_RELEASE_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int occ_time;
    int punish_type;
    signed char cert_flag;
    signed char cert_type;
    char reason[201];
    signed char apply_flag;
} ProtectAccReleaseLog;

/* CRUD Operations */
int ProtectAccReleaseLog_Add(DBConnectionManager* manager, const ProtectAccReleaseLog* record);
int ProtectAccReleaseLog_Get(DBConnectionManager* manager, int no, ProtectAccReleaseLog* record);
int ProtectAccReleaseLog_Update(DBConnectionManager* manager, const ProtectAccReleaseLog* record);
int ProtectAccReleaseLog_Delete(DBConnectionManager* manager, int no);
int ProtectAccReleaseLog_Exists(DBConnectionManager* manager, int no);
int ProtectAccReleaseLog_GetAll(DBConnectionManager* manager, ProtectAccReleaseLog* records, int max_count, int* actual_count);

#endif /* PROTECT_ACC_RELEASE_LOG_ORM_H */