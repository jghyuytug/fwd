#ifndef MEMBER_SECURITY_CARD_ORM_H
#define MEMBER_SECURITY_CARD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    char phone[12];
    char cert_key[13];
    char server_key[33];
    char card[256];
    signed char fail_cnt;
    signed char re_issue_cnt;
    char last_issue_time[20];
    int validity_time;
    signed char apply_flag;
    unsigned short cancel_cnt;
    signed char web_flag;
    char cert_flag;
} MemberSecurityCard;

/* CRUD Operations */
int MemberSecurityCard_Add(DBConnectionManager* manager, const MemberSecurityCard* card);
int MemberSecurityCard_Get(DBConnectionManager* manager, int m_id, MemberSecurityCard* card);
int MemberSecurityCard_Update(DBConnectionManager* manager, const MemberSecurityCard* card);
int MemberSecurityCard_Delete(DBConnectionManager* manager, int m_id);
int MemberSecurityCard_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int MemberSecurityCard_GetByPhone(DBConnectionManager* manager, const char* phone,
                                   MemberSecurityCard* cards, int max_count, int* actual_count);
int MemberSecurityCard_UpdateFailCount(DBConnectionManager* manager, int m_id, signed char fail_cnt);
int MemberSecurityCard_IncrementReissueCount(DBConnectionManager* manager, int m_id);
int MemberSecurityCard_ResetCard(DBConnectionManager* manager, int m_id, const char* new_card,
                                  const char* new_server_key);

/* Utility */
void MemberSecurityCard_PrintInfo(const MemberSecurityCard* card);

#endif
