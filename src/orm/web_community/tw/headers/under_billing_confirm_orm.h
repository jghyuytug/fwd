#ifndef UNDER_BILLING_CONFIRM_ORM_H
#define UNDER_BILLING_CONFIRM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char parent_name[5];
    long long parent_jumin;
    signed char parent_phone1;
    short parent_phone2;
    short parent_phone3;
    char parent_email[26];
    signed char parent_consent_type;
    int create_date;
    int consent_date;
    signed char consent_yn;
} UnderBillingConfirm;

/* CRUD Operations */
int UnderBillingConfirm_Add(DBConnectionManager* manager, const UnderBillingConfirm* record);
int UnderBillingConfirm_Get(DBConnectionManager* manager, int m_id, UnderBillingConfirm* record);
int UnderBillingConfirm_Update(DBConnectionManager* manager, const UnderBillingConfirm* record);
int UnderBillingConfirm_Delete(DBConnectionManager* manager, int m_id);
int UnderBillingConfirm_Exists(DBConnectionManager* manager, int m_id);
int UnderBillingConfirm_GetAll(DBConnectionManager* manager, UnderBillingConfirm* records, int max_count, int* actual_count);

#endif /* UNDER_BILLING_CONFIRM_ORM_H */