#ifndef MEMBER_MOUSE_SMS_ORM_H
#define MEMBER_MOUSE_SMS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    signed char cnt;
} MemberMouseSms;

/* CRUD Operations */
int MemberMouseSms_Add(DBConnectionManager* manager, const MemberMouseSms* record);
int MemberMouseSms_Get(DBConnectionManager* manager, int m_id, MemberMouseSms* record);
int MemberMouseSms_Update(DBConnectionManager* manager, const MemberMouseSms* record);
int MemberMouseSms_Delete(DBConnectionManager* manager, int m_id);
int MemberMouseSms_Exists(DBConnectionManager* manager, int m_id);
int MemberMouseSms_GetAll(DBConnectionManager* manager, MemberMouseSms* records, int max_count, int* actual_count);

#endif /* MEMBER_MOUSE_SMS_ORM_H */