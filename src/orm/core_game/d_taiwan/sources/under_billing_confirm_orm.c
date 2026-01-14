#include "under_billing_confirm_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int UnderBillingConfirm_Add(DBConnectionManager* manager, const UnderBillingConfirm* confirm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !confirm) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO under_billing_confirm "
        "(m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn) "
        "VALUES (%u, '%s', %llu, %u, %u, %u, '%s', %u, %u, %u, %u)",
        confirm->m_id, confirm->parent_name, confirm->parent_jumin,
        confirm->parent_phone1, confirm->parent_phone2, confirm->parent_phone3,
        confirm->parent_email, confirm->parent_consent_type,
        confirm->create_date, confirm->consent_date, confirm->consent_yn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_Get(DBConnectionManager* manager, unsigned int m_id,
                            UnderBillingConfirm* confirm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !confirm) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn "
        "FROM under_billing_confirm WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(confirm, 0, sizeof(UnderBillingConfirm));
    confirm->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(confirm->parent_name, row[1], sizeof(confirm->parent_name) - 1);
    confirm->parent_jumin = row[2] ? (unsigned long long)strtoull(row[2], NULL, 10) : 0;
    confirm->parent_phone1 = row[3] ? (unsigned char)atoi(row[3]) : 0;
    confirm->parent_phone2 = row[4] ? (unsigned short)atoi(row[4]) : 0;
    confirm->parent_phone3 = row[5] ? (unsigned short)atoi(row[5]) : 0;
    if (row[6]) strncpy(confirm->parent_email, row[6], sizeof(confirm->parent_email) - 1);
    confirm->parent_consent_type = row[7] ? (unsigned char)atoi(row[7]) : 0;
    confirm->create_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
    confirm->consent_date = row[9] ? (unsigned int)atoi(row[9]) : 0;
    confirm->consent_yn = row[10] ? (unsigned char)atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_Update(DBConnectionManager* manager, const UnderBillingConfirm* confirm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !confirm) return -1;

    snprintf(query, sizeof(query),
        "UPDATE under_billing_confirm SET "
        "parent_name = '%s', parent_jumin = %llu, "
        "parent_phone1 = %u, parent_phone2 = %u, parent_phone3 = %u, "
        "parent_email = '%s', parent_consent_type = %u, "
        "consent_date = %u, consent_yn = %u "
        "WHERE m_id = %u",
        confirm->parent_name, confirm->parent_jumin,
        confirm->parent_phone1, confirm->parent_phone2, confirm->parent_phone3,
        confirm->parent_email, confirm->parent_consent_type,
        confirm->consent_date, confirm->consent_yn,
        confirm->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM under_billing_confirm WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM under_billing_confirm WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int UnderBillingConfirm_GetAll(DBConnectionManager* manager, UnderBillingConfirm* confirms,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !confirms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn "
        "FROM under_billing_confirm ORDER BY create_date DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&confirms[count], 0, sizeof(UnderBillingConfirm));
        confirms[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(confirms[count].parent_name, row[1], sizeof(confirms[count].parent_name) - 1);
        confirms[count].parent_jumin = row[2] ? (unsigned long long)strtoull(row[2], NULL, 10) : 0;
        confirms[count].parent_phone1 = row[3] ? (unsigned char)atoi(row[3]) : 0;
        confirms[count].parent_phone2 = row[4] ? (unsigned short)atoi(row[4]) : 0;
        confirms[count].parent_phone3 = row[5] ? (unsigned short)atoi(row[5]) : 0;
        if (row[6]) strncpy(confirms[count].parent_email, row[6], sizeof(confirms[count].parent_email) - 1);
        confirms[count].parent_consent_type = row[7] ? (unsigned char)atoi(row[7]) : 0;
        confirms[count].create_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        confirms[count].consent_date = row[9] ? (unsigned int)atoi(row[9]) : 0;
        confirms[count].consent_yn = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_GetByParentName(DBConnectionManager* manager, const char* parent_name,
                                        UnderBillingConfirm* confirms, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !parent_name || !confirms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn "
        "FROM under_billing_confirm WHERE parent_name = '%s' ORDER BY create_date DESC",
        parent_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&confirms[count], 0, sizeof(UnderBillingConfirm));
        confirms[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(confirms[count].parent_name, row[1], sizeof(confirms[count].parent_name) - 1);
        confirms[count].parent_jumin = row[2] ? (unsigned long long)strtoull(row[2], NULL, 10) : 0;
        confirms[count].parent_phone1 = row[3] ? (unsigned char)atoi(row[3]) : 0;
        confirms[count].parent_phone2 = row[4] ? (unsigned short)atoi(row[4]) : 0;
        confirms[count].parent_phone3 = row[5] ? (unsigned short)atoi(row[5]) : 0;
        if (row[6]) strncpy(confirms[count].parent_email, row[6], sizeof(confirms[count].parent_email) - 1);
        confirms[count].parent_consent_type = row[7] ? (unsigned char)atoi(row[7]) : 0;
        confirms[count].create_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        confirms[count].consent_date = row[9] ? (unsigned int)atoi(row[9]) : 0;
        confirms[count].consent_yn = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_GetByParentEmail(DBConnectionManager* manager, const char* parent_email,
                                         UnderBillingConfirm* confirms, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !parent_email || !confirms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn "
        "FROM under_billing_confirm WHERE parent_email = '%s' ORDER BY create_date DESC",
        parent_email);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&confirms[count], 0, sizeof(UnderBillingConfirm));
        confirms[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(confirms[count].parent_name, row[1], sizeof(confirms[count].parent_name) - 1);
        confirms[count].parent_jumin = row[2] ? (unsigned long long)strtoull(row[2], NULL, 10) : 0;
        confirms[count].parent_phone1 = row[3] ? (unsigned char)atoi(row[3]) : 0;
        confirms[count].parent_phone2 = row[4] ? (unsigned short)atoi(row[4]) : 0;
        confirms[count].parent_phone3 = row[5] ? (unsigned short)atoi(row[5]) : 0;
        if (row[6]) strncpy(confirms[count].parent_email, row[6], sizeof(confirms[count].parent_email) - 1);
        confirms[count].parent_consent_type = row[7] ? (unsigned char)atoi(row[7]) : 0;
        confirms[count].create_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        confirms[count].consent_date = row[9] ? (unsigned int)atoi(row[9]) : 0;
        confirms[count].consent_yn = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_GetByConsentStatus(DBConnectionManager* manager, unsigned char consent_yn,
                                            UnderBillingConfirm* confirms, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !confirms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn "
        "FROM under_billing_confirm WHERE consent_yn = %u ORDER BY create_date DESC",
        consent_yn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&confirms[count], 0, sizeof(UnderBillingConfirm));
        confirms[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(confirms[count].parent_name, row[1], sizeof(confirms[count].parent_name) - 1);
        confirms[count].parent_jumin = row[2] ? (unsigned long long)strtoull(row[2], NULL, 10) : 0;
        confirms[count].parent_phone1 = row[3] ? (unsigned char)atoi(row[3]) : 0;
        confirms[count].parent_phone2 = row[4] ? (unsigned short)atoi(row[4]) : 0;
        confirms[count].parent_phone3 = row[5] ? (unsigned short)atoi(row[5]) : 0;
        if (row[6]) strncpy(confirms[count].parent_email, row[6], sizeof(confirms[count].parent_email) - 1);
        confirms[count].parent_consent_type = row[7] ? (unsigned char)atoi(row[7]) : 0;
        confirms[count].create_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        confirms[count].consent_date = row[9] ? (unsigned int)atoi(row[9]) : 0;
        confirms[count].consent_yn = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_GetByDateRange(DBConnectionManager* manager,
                                        unsigned int start_date, unsigned int end_date,
                                        UnderBillingConfirm* confirms, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !confirms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, "
        "parent_email, parent_consent_type, create_date, consent_date, consent_yn "
        "FROM under_billing_confirm WHERE create_date BETWEEN %u AND %u ORDER BY create_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&confirms[count], 0, sizeof(UnderBillingConfirm));
        confirms[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(confirms[count].parent_name, row[1], sizeof(confirms[count].parent_name) - 1);
        confirms[count].parent_jumin = row[2] ? (unsigned long long)strtoull(row[2], NULL, 10) : 0;
        confirms[count].parent_phone1 = row[3] ? (unsigned char)atoi(row[3]) : 0;
        confirms[count].parent_phone2 = row[4] ? (unsigned short)atoi(row[4]) : 0;
        confirms[count].parent_phone3 = row[5] ? (unsigned short)atoi(row[5]) : 0;
        if (row[6]) strncpy(confirms[count].parent_email, row[6], sizeof(confirms[count].parent_email) - 1);
        confirms[count].parent_consent_type = row[7] ? (unsigned char)atoi(row[7]) : 0;
        confirms[count].create_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        confirms[count].consent_date = row[9] ? (unsigned int)atoi(row[9]) : 0;
        confirms[count].consent_yn = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void UnderBillingConfirm_PrintInfo(const UnderBillingConfirm* confirm) {
    if (!confirm) return;

    printf("=== Under Billing Confirm Info ===\n");
    printf("Member ID: %u\n", confirm->m_id);
    printf("Parent Name: %s\n", confirm->parent_name);
    printf("Parent Jumin: %llu\n", confirm->parent_jumin);
    printf("Parent Phone: %03u-%04u-%04u\n",
           confirm->parent_phone1, confirm->parent_phone2, confirm->parent_phone3);
    printf("Parent Email: %s\n", confirm->parent_email);
    printf("Parent Consent Type: %u\n", confirm->parent_consent_type);
    printf("Create Date: %u\n", confirm->create_date);
    printf("Consent Date: %u\n", confirm->consent_date);
    printf("Consent YN: %u\n", confirm->consent_yn);
    printf("==================================\n");
}
