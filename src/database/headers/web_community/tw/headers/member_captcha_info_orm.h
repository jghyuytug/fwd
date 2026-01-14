#ifndef MEMBER_CAPTCHA_INFO_ORM_H
#define MEMBER_CAPTCHA_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int cert_time;
    signed char fail_count;
} MemberCaptchaInfo;

/* CRUD Operations */
int MemberCaptchaInfo_Add(DBConnectionManager* manager, const MemberCaptchaInfo* record);
int MemberCaptchaInfo_Get(DBConnectionManager* manager, int m_id, MemberCaptchaInfo* record);
int MemberCaptchaInfo_Update(DBConnectionManager* manager, const MemberCaptchaInfo* record);
int MemberCaptchaInfo_Delete(DBConnectionManager* manager, int m_id);
int MemberCaptchaInfo_Exists(DBConnectionManager* manager, int m_id);
int MemberCaptchaInfo_GetAll(DBConnectionManager* manager, MemberCaptchaInfo* records, int max_count, int* actual_count);

#endif /* MEMBER_CAPTCHA_INFO_ORM_H */