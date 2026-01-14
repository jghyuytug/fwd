#ifndef MEMBER_MOUSEPASS_ORM_H
#define MEMBER_MOUSEPASS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char mousepass[33];        /* VARCHAR(32) -> 33 */
    char occ_time[20];         /* DATETIME */
    unsigned char fail_cnt;    /* TINYINT(4) */
    unsigned short cancel_cnt; /* SMALLINT(5) UNSIGNED */
    char version_info;         /* CHAR(1) */
    int validity_time;
    int reward_time;
    char enable_flag;          /* CHAR(1) */
} MemberMousepass;

/* CRUD operations */
int MemberMousepass_Add(DBConnectionManager* manager, const MemberMousepass* record);
int MemberMousepass_Get(DBConnectionManager* manager, int m_id, MemberMousepass* record);
int MemberMousepass_Update(DBConnectionManager* manager, const MemberMousepass* record);
int MemberMousepass_Delete(DBConnectionManager* manager, int m_id);
int MemberMousepass_Exists(DBConnectionManager* manager, int m_id);

/* Business operations */
int MemberMousepass_GetByEnableFlag(DBConnectionManager* manager, char enable_flag,
                                     MemberMousepass* records, int max_count, int* actual_count);
int MemberMousepass_UpdateFailCount(DBConnectionManager* manager, int m_id, unsigned char fail_cnt);
int MemberMousepass_ResetFailCount(DBConnectionManager* manager, int m_id);
int MemberMousepass_IncrementCancelCount(DBConnectionManager* manager, int m_id);

/* Utility */
void MemberMousepass_PrintInfo(const MemberMousepass* record);

#endif
