#ifndef CLEANUP_PENALTY_LOG_ORM_H
#define CLEANUP_PENALTY_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    signed char server_info;
    char occ_date[20];
    signed char penalty_point;
    int current_point;
    signed char reset_cnt;
    signed char cause;
    signed char pattern;
} CleanupPenaltyLog;

/* CRUD Operations */
int CleanupPenaltyLog_Add(DBConnectionManager* manager, const CleanupPenaltyLog* record);
int CleanupPenaltyLog_GetAll(DBConnectionManager* manager, CleanupPenaltyLog* records, int max_count, int* actual_count);

#endif /* CLEANUP_PENALTY_LOG_ORM_H */