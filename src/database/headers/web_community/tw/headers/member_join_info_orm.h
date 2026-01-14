#ifndef MEMBER_JOIN_INFO_ORM_H
#define MEMBER_JOIN_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int reg_date;
    char ip[16];
    signed char contry_code;
    int login_time;
    signed char error_type;
    char login_ip[16];
    signed char game_use_history;
} MemberJoinInfo;

/* CRUD Operations */
int MemberJoinInfo_Add(DBConnectionManager* manager, const MemberJoinInfo* record);
int MemberJoinInfo_Get(DBConnectionManager* manager, int m_id, MemberJoinInfo* record);
int MemberJoinInfo_Update(DBConnectionManager* manager, const MemberJoinInfo* record);
int MemberJoinInfo_Delete(DBConnectionManager* manager, int m_id);
int MemberJoinInfo_Exists(DBConnectionManager* manager, int m_id);
int MemberJoinInfo_GetAll(DBConnectionManager* manager, MemberJoinInfo* records, int max_count, int* actual_count);

#endif /* MEMBER_JOIN_INFO_ORM_H */