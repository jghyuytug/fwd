#ifndef MEMBER_PREMIUM_OLD_ORM_H
#define MEMBER_PREMIUM_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    unsigned char pre_type;
    int m_id;
    char service_start[20];
    char service_end[20];
    unsigned char server_id;
} MemberPremiumOld;

/* CRUD Operations */
int MemberPremiumOld_Add(DBConnectionManager* manager, const MemberPremiumOld* premium);
int MemberPremiumOld_Get(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                          unsigned char server_id, int m_id, const char* service_start,
                          MemberPremiumOld* premium);
int MemberPremiumOld_Update(DBConnectionManager* manager, const MemberPremiumOld* premium);
int MemberPremiumOld_Delete(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                             unsigned char server_id, int m_id, const char* service_start);
int MemberPremiumOld_Exists(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                             unsigned char server_id, int m_id, const char* service_start);

/* Business Operations */
int MemberPremiumOld_GetByMember(DBConnectionManager* manager, int m_id,
                                  MemberPremiumOld* premiums, int max_count, int* actual_count);
int MemberPremiumOld_GetByEvent(DBConnectionManager* manager, int event_id,
                                 MemberPremiumOld* premiums, int max_count, int* actual_count);

/* Utility */
void MemberPremiumOld_PrintInfo(const MemberPremiumOld* premium);

#endif
