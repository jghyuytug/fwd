#ifndef LOGIN_HISTORY_ORM_H
#define LOGIN_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;              /* INT - Unix timestamp */
    unsigned char trigger_type; /* TINYINT(4) - renamed from 'trigger' */
} LoginHistory;

/* Operations - no primary key */
int LoginHistory_Add(DBConnectionManager* manager, const LoginHistory* record);
int LoginHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                LoginHistory* records, int max_count, int* actual_count);
int LoginHistory_GetByTimeRange(DBConnectionManager* manager,
                                 int start_time, int end_time,
                                 LoginHistory* records, int max_count, int* actual_count);
int LoginHistory_GetByTrigger(DBConnectionManager* manager, unsigned char trigger_type,
                               LoginHistory* records, int max_count, int* actual_count);
int LoginHistory_DeleteOldRecords(DBConnectionManager* manager, int before_time);
int LoginHistory_GetRecent(DBConnectionManager* manager, int limit,
                            LoginHistory* records, int max_count, int* actual_count);

/* Utility */
void LoginHistory_PrintInfo(const LoginHistory* record);

#endif
