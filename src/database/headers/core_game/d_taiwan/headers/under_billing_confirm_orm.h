#ifndef UNDER_BILLING_CONFIRM_ORM_H
#define UNDER_BILLING_CONFIRM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
    char parent_name[5];
    unsigned long long parent_jumin;
    unsigned char parent_phone1;
    unsigned short parent_phone2;
    unsigned short parent_phone3;
    char parent_email[26];
    unsigned char parent_consent_type;
    unsigned int create_date;
    unsigned int consent_date;
    unsigned char consent_yn;
} UnderBillingConfirm;

/* CRUD Operations */
int UnderBillingConfirm_Add(DBConnectionManager* manager, const UnderBillingConfirm* confirm);
int UnderBillingConfirm_Get(DBConnectionManager* manager, unsigned int m_id,
                            UnderBillingConfirm* confirm);
int UnderBillingConfirm_Update(DBConnectionManager* manager, const UnderBillingConfirm* confirm);
int UnderBillingConfirm_Delete(DBConnectionManager* manager, unsigned int m_id);
int UnderBillingConfirm_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int UnderBillingConfirm_GetAll(DBConnectionManager* manager, UnderBillingConfirm* confirms,
                               int max_count, int* actual_count);
int UnderBillingConfirm_GetByParentName(DBConnectionManager* manager, const char* parent_name,
                                        UnderBillingConfirm* confirms, int max_count, int* actual_count);
int UnderBillingConfirm_GetByParentEmail(DBConnectionManager* manager, const char* parent_email,
                                         UnderBillingConfirm* confirms, int max_count, int* actual_count);
int UnderBillingConfirm_GetByConsentStatus(DBConnectionManager* manager, unsigned char consent_yn,
                                            UnderBillingConfirm* confirms, int max_count, int* actual_count);
int UnderBillingConfirm_GetByDateRange(DBConnectionManager* manager,
                                        unsigned int start_date, unsigned int end_date,
                                        UnderBillingConfirm* confirms, int max_count, int* actual_count);

/* Utility Functions */
void UnderBillingConfirm_PrintInfo(const UnderBillingConfirm* confirm);

#endif
