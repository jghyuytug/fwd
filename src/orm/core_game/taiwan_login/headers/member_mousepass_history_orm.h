#ifndef MEMBER_MOUSEPASS_HISTORY_ORM_H
#define MEMBER_MOUSEPASS_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];         /* DATETIME */
    int m_id;
    char pre_mousepass[33];    /* VARCHAR(32) -> 33 */
    unsigned char modify_type; /* TINYINT(4) */
} MemberMousepassHistory;

/* Operations - no primary key, use combined fields */
int MemberMousepassHistory_Add(DBConnectionManager* manager, const MemberMousepassHistory* record);
int MemberMousepassHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                          MemberMousepassHistory* records,
                                          int max_count, int* actual_count);
int MemberMousepassHistory_GetByTimeRange(DBConnectionManager* manager,
                                           const char* start_time, const char* end_time,
                                           MemberMousepassHistory* records,
                                           int max_count, int* actual_count);
int MemberMousepassHistory_GetByModifyType(DBConnectionManager* manager,
                                            unsigned char modify_type,
                                            MemberMousepassHistory* records,
                                            int max_count, int* actual_count);
int MemberMousepassHistory_DeleteOldRecords(DBConnectionManager* manager, const char* before_time);

/* Utility */
void MemberMousepassHistory_PrintInfo(const MemberMousepassHistory* record);

#endif
