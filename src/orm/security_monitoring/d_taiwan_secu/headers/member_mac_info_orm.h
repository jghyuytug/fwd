#ifndef MEMBER_MAC_INFO_ORM_H
#define MEMBER_MAC_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char mac_addr[65];
    char occ_time[20];
} MemberMacInfo;

/* CRUD Operations */
int MemberMacInfo_Add(DBConnectionManager* manager, const MemberMacInfo* record);
int MemberMacInfo_Get(DBConnectionManager* manager, int no, MemberMacInfo* record);
int MemberMacInfo_Update(DBConnectionManager* manager, const MemberMacInfo* record);
int MemberMacInfo_Delete(DBConnectionManager* manager, int no);
int MemberMacInfo_Exists(DBConnectionManager* manager, int no);
int MemberMacInfo_GetAll(DBConnectionManager* manager, MemberMacInfo* records, int max_count, int* actual_count);

#endif /* MEMBER_MAC_INFO_ORM_H */