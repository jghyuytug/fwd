#ifndef UNDER_AGE_CONSENT_ORM_H
#define UNDER_AGE_CONSENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
    unsigned char consent_type;
    unsigned int limit_money;
    char parent_name[5];
    unsigned long long parent_jumin;
    unsigned char parent_phone1;
    unsigned short parent_phone2;
    unsigned short parent_phone3;
    char parent_email[26];
    unsigned char parent_consent_type;
    unsigned char notice_type;
    char notice_addr[51];
    unsigned int create_date;
    unsigned int consent_date;
    unsigned char consent_yn;
    unsigned char history_yn;
} UnderAgeConsent;

/* CRUD Operations */
int UnderAgeConsent_Add(DBConnectionManager* manager, const UnderAgeConsent* consent);
int UnderAgeConsent_Get(DBConnectionManager* manager, unsigned int m_id,
                        unsigned int create_date, UnderAgeConsent* consent);
int UnderAgeConsent_Update(DBConnectionManager* manager, const UnderAgeConsent* consent);
int UnderAgeConsent_Delete(DBConnectionManager* manager, unsigned int m_id,
                           unsigned int create_date);
int UnderAgeConsent_Exists(DBConnectionManager* manager, unsigned int m_id,
                           unsigned int create_date);

/* Business Operations */
int UnderAgeConsent_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                   UnderAgeConsent* consents, int max_count, int* actual_count);
int UnderAgeConsent_GetByParentName(DBConnectionManager* manager, const char* parent_name,
                                     UnderAgeConsent* consents, int max_count, int* actual_count);
int UnderAgeConsent_GetByParentEmail(DBConnectionManager* manager, const char* parent_email,
                                      UnderAgeConsent* consents, int max_count, int* actual_count);
int UnderAgeConsent_GetByConsentStatus(DBConnectionManager* manager, unsigned char consent_yn,
                                        UnderAgeConsent* consents, int max_count, int* actual_count);
int UnderAgeConsent_GetAll(DBConnectionManager* manager, UnderAgeConsent* consents,
                           int max_count, int* actual_count);

/* Utility Functions */
void UnderAgeConsent_PrintInfo(const UnderAgeConsent* consent);

#endif
