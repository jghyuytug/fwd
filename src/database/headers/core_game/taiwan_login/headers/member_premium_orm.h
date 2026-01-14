#ifndef MEMBER_PREMIUM_ORM_H
#define MEMBER_PREMIUM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    unsigned char pre_type;
    unsigned char server_id;
    int m_id;
    char service_start[20];
    char service_end[20];
} MemberPremium;

/* CRUD Operations */
int MemberPremium_Add(DBConnectionManager* manager, const MemberPremium* premium);
int MemberPremium_Get(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                       unsigned char server_id, int m_id, const char* service_start,
                       MemberPremium* premium);
int MemberPremium_Update(DBConnectionManager* manager, const MemberPremium* premium);
int MemberPremium_Delete(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                          unsigned char server_id, int m_id, const char* service_start);
int MemberPremium_Exists(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                          unsigned char server_id, int m_id, const char* service_start);

/* Business Operations */
int MemberPremium_GetByMember(DBConnectionManager* manager, int m_id,
                               MemberPremium* premiums, int max_count, int* actual_count);
int MemberPremium_GetByEvent(DBConnectionManager* manager, int event_id,
                              MemberPremium* premiums, int max_count, int* actual_count);
int MemberPremium_GetActive(DBConnectionManager* manager, int m_id, const char* current_time,
                             MemberPremium* premiums, int max_count, int* actual_count);

/* Utility */
void MemberPremium_PrintInfo(const MemberPremium* premium);

#endif
