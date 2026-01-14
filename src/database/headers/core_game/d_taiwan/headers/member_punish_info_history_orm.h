#ifndef MEMBER_PUNISH_INFO_HISTORY_ORM_H
#define MEMBER_PUNISH_INFO_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;            /* AUTO_INCREMENT primary key */
    int m_id;
    int punish_type;
    char occ_time[20];          /* DATETIME */
    int punish_value;
    unsigned char apply_flag;   /* tinyint(4) */
    char start_time[20];        /* DATETIME */
    char end_time[20];          /* DATETIME */
    char admin_id[26];          /* VARCHAR(25) -> 26 */
    char reason[101];           /* VARCHAR(100) -> 101 */
    unsigned char is_kicked;    /* tinyint(4) */
    char first_ssn[33];         /* VARCHAR(32) -> 33 */
    char second_ssn[33];        /* VARCHAR(32) -> 33 */
} MemberPunishInfoHistory;

/* CRUD Operations */
int MemberPunishInfoHistory_Add(DBConnectionManager* manager, const MemberPunishInfoHistory* record);
int MemberPunishInfoHistory_Get(DBConnectionManager* manager, unsigned int no, MemberPunishInfoHistory* record);
int MemberPunishInfoHistory_Update(DBConnectionManager* manager, const MemberPunishInfoHistory* record);
int MemberPunishInfoHistory_Delete(DBConnectionManager* manager, unsigned int no);
int MemberPunishInfoHistory_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int MemberPunishInfoHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                           MemberPunishInfoHistory* records, int max_count, int* actual_count);
int MemberPunishInfoHistory_GetByPunishType(DBConnectionManager* manager, int punish_type,
                                             MemberPunishInfoHistory* records, int max_count, int* actual_count);
int MemberPunishInfoHistory_GetByAdmin(DBConnectionManager* manager, const char* admin_id,
                                        MemberPunishInfoHistory* records, int max_count, int* actual_count);
int MemberPunishInfoHistory_GetByTimeRange(DBConnectionManager* manager, const char* start_time, const char* end_time,
                                            MemberPunishInfoHistory* records, int max_count, int* actual_count);
int MemberPunishInfoHistory_DeleteByMemberId(DBConnectionManager* manager, int m_id);

/* Utility Functions */
void MemberPunishInfoHistory_PrintInfo(const MemberPunishInfoHistory* record);

#endif /* MEMBER_PUNISH_INFO_HISTORY_ORM_H */
