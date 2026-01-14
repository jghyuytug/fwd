#ifndef UNDER_AGE_CONSENT_ORM_H
#define UNDER_AGE_CONSENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char consent_type;
    int limit_money;
    char parent_name[5];
    long long parent_jumin;
    signed char parent_phone1;
    short parent_phone2;
    short parent_phone3;
    char parent_email[26];
    signed char parent_consent_type;
    signed char notice_type;
    char notice_addr[51];
    int create_date;
    int consent_date;
    signed char consent_yn;
    signed char history_yn;
} UnderAgeConsent;

/* CRUD Operations */
int UnderAgeConsent_Add(DBConnectionManager* manager, const UnderAgeConsent* record);
int UnderAgeConsent_GetAll(DBConnectionManager* manager, UnderAgeConsent* records, int max_count, int* actual_count);

#endif /* UNDER_AGE_CONSENT_ORM_H */