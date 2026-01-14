#ifndef MEMBER_MOUSEPASS_HISTORY_201506_ORM_H
#define MEMBER_MOUSEPASS_HISTORY_201506_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];         /* DATETIME */
    int m_id;
    char pre_mousepass[33];    /* VARCHAR(32) -> 33 */
    unsigned char modify_type; /* TINYINT(4) */
    char ip_info[16];          /* VARCHAR(15) -> 16 */
    char port_info[6];         /* VARCHAR(5) -> 6 */
} MemberMousepassHistory201506;

/* CRUD operations - composite primary key (occ_time, m_id) */
int MemberMousepassHistory201506_Add(DBConnectionManager* manager, const MemberMousepassHistory201506* record);
int MemberMousepassHistory201506_Get(DBConnectionManager* manager,
                                      const char* occ_time, int m_id,
                                      MemberMousepassHistory201506* record);
int MemberMousepassHistory201506_Delete(DBConnectionManager* manager,
                                         const char* occ_time, int m_id);
int MemberMousepassHistory201506_Exists(DBConnectionManager* manager,
                                         const char* occ_time, int m_id);

/* Business operations */
int MemberMousepassHistory201506_GetByMemberId(DBConnectionManager* manager, int m_id,
                                                MemberMousepassHistory201506* records,
                                                int max_count, int* actual_count);
int MemberMousepassHistory201506_GetByIP(DBConnectionManager* manager, const char* ip_info,
                                          MemberMousepassHistory201506* records,
                                          int max_count, int* actual_count);
int MemberMousepassHistory201506_GetByTimeRange(DBConnectionManager* manager,
                                                 const char* start_time, const char* end_time,
                                                 MemberMousepassHistory201506* records,
                                                 int max_count, int* actual_count);

/* Utility */
void MemberMousepassHistory201506_PrintInfo(const MemberMousepassHistory201506* record);

#endif
