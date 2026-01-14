#ifndef MEMBER_INFO_DETAIL_ORM_H
#define MEMBER_INFO_DETAIL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char zipcode[8];
    char address[81];
    char address_detail[71];
    char occ_date[20];
} MemberInfoDetail;

/* CRUD Operations */
int MemberInfoDetail_Add(DBConnectionManager* manager, const MemberInfoDetail* record);
int MemberInfoDetail_Get(DBConnectionManager* manager, int m_id, MemberInfoDetail* record);
int MemberInfoDetail_Update(DBConnectionManager* manager, const MemberInfoDetail* record);
int MemberInfoDetail_Delete(DBConnectionManager* manager, int m_id);
int MemberInfoDetail_Exists(DBConnectionManager* manager, int m_id);
int MemberInfoDetail_GetAll(DBConnectionManager* manager, MemberInfoDetail* records, int max_count, int* actual_count);

#endif /* MEMBER_INFO_DETAIL_ORM_H */