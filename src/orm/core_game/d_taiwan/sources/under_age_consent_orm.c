#include "under_age_consent_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UnderAgeConsent_Add(DBConnectionManager* manager, const UnderAgeConsent* consent) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !consent) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO under_age_consent "
        "(m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn) "
        "VALUES (%u, %u, %u, '%s', %llu, %u, %u, %u, '%s', %u, %u, '%s', %u, %u, %u, %u)",
        consent->m_id, consent->consent_type, consent->limit_money,
        consent->parent_name, consent->parent_jumin,
        consent->parent_phone1, consent->parent_phone2, consent->parent_phone3,
        consent->parent_email, consent->parent_consent_type,
        consent->notice_type, consent->notice_addr,
        consent->create_date, consent->consent_date,
        consent->consent_yn, consent->history_yn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_Get(DBConnectionManager* manager, unsigned int m_id,
                        unsigned int create_date, UnderAgeConsent* consent) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];

    if (!manager || !consent) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn "
        "FROM under_age_consent WHERE m_id = %u AND create_date = %u",
        m_id, create_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(consent, 0, sizeof(UnderAgeConsent));
    consent->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    consent->consent_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
    consent->limit_money = row[2] ? (unsigned int)atoi(row[2]) : 0;
    if (row[3]) strncpy(consent->parent_name, row[3], sizeof(consent->parent_name) - 1);
    consent->parent_jumin = row[4] ? (unsigned long long)strtoull(row[4], NULL, 10) : 0;
    consent->parent_phone1 = row[5] ? (unsigned char)atoi(row[5]) : 0;
    consent->parent_phone2 = row[6] ? (unsigned short)atoi(row[6]) : 0;
    consent->parent_phone3 = row[7] ? (unsigned short)atoi(row[7]) : 0;
    if (row[8]) strncpy(consent->parent_email, row[8], sizeof(consent->parent_email) - 1);
    consent->parent_consent_type = row[9] ? (unsigned char)atoi(row[9]) : 0;
    consent->notice_type = row[10] ? (unsigned char)atoi(row[10]) : 0;
    if (row[11]) strncpy(consent->notice_addr, row[11], sizeof(consent->notice_addr) - 1);
    consent->create_date = row[12] ? (unsigned int)atoi(row[12]) : 0;
    consent->consent_date = row[13] ? (unsigned int)atoi(row[13]) : 0;
    consent->consent_yn = row[14] ? (unsigned char)atoi(row[14]) : 0;
    consent->history_yn = row[15] ? (unsigned char)atoi(row[15]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_Update(DBConnectionManager* manager, const UnderAgeConsent* consent) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !consent) return -1;

    snprintf(query, sizeof(query),
        "UPDATE under_age_consent SET "
        "consent_type = %u, limit_money = %u, parent_name = '%s', "
        "parent_jumin = %llu, parent_phone1 = %u, parent_phone2 = %u, "
        "parent_phone3 = %u, parent_email = '%s', parent_consent_type = %u, "
        "notice_type = %u, notice_addr = '%s', consent_date = %u, "
        "consent_yn = %u, history_yn = %u "
        "WHERE m_id = %u AND create_date = %u",
        consent->consent_type, consent->limit_money, consent->parent_name,
        consent->parent_jumin, consent->parent_phone1, consent->parent_phone2,
        consent->parent_phone3, consent->parent_email, consent->parent_consent_type,
        consent->notice_type, consent->notice_addr, consent->consent_date,
        consent->consent_yn, consent->history_yn,
        consent->m_id, consent->create_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_Delete(DBConnectionManager* manager, unsigned int m_id,
                           unsigned int create_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM under_age_consent WHERE m_id = %u AND create_date = %u",
        m_id, create_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_Exists(DBConnectionManager* manager, unsigned int m_id,
                           unsigned int create_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM under_age_consent WHERE m_id = %u AND create_date = %u",
        m_id, create_date);

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

int UnderAgeConsent_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                   UnderAgeConsent* consents, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !consents || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn "
        "FROM under_age_consent WHERE m_id = %u ORDER BY create_date DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&consents[count], 0, sizeof(UnderAgeConsent));
        consents[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        consents[count].consent_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        consents[count].limit_money = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(consents[count].parent_name, row[3], sizeof(consents[count].parent_name) - 1);
        consents[count].parent_jumin = row[4] ? (unsigned long long)strtoull(row[4], NULL, 10) : 0;
        consents[count].parent_phone1 = row[5] ? (unsigned char)atoi(row[5]) : 0;
        consents[count].parent_phone2 = row[6] ? (unsigned short)atoi(row[6]) : 0;
        consents[count].parent_phone3 = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(consents[count].parent_email, row[8], sizeof(consents[count].parent_email) - 1);
        consents[count].parent_consent_type = row[9] ? (unsigned char)atoi(row[9]) : 0;
        consents[count].notice_type = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(consents[count].notice_addr, row[11], sizeof(consents[count].notice_addr) - 1);
        consents[count].create_date = row[12] ? (unsigned int)atoi(row[12]) : 0;
        consents[count].consent_date = row[13] ? (unsigned int)atoi(row[13]) : 0;
        consents[count].consent_yn = row[14] ? (unsigned char)atoi(row[14]) : 0;
        consents[count].history_yn = row[15] ? (unsigned char)atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_GetByParentName(DBConnectionManager* manager, const char* parent_name,
                                     UnderAgeConsent* consents, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !parent_name || !consents || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn "
        "FROM under_age_consent WHERE parent_name = '%s' ORDER BY create_date DESC",
        parent_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&consents[count], 0, sizeof(UnderAgeConsent));
        consents[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        consents[count].consent_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        consents[count].limit_money = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(consents[count].parent_name, row[3], sizeof(consents[count].parent_name) - 1);
        consents[count].parent_jumin = row[4] ? (unsigned long long)strtoull(row[4], NULL, 10) : 0;
        consents[count].parent_phone1 = row[5] ? (unsigned char)atoi(row[5]) : 0;
        consents[count].parent_phone2 = row[6] ? (unsigned short)atoi(row[6]) : 0;
        consents[count].parent_phone3 = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(consents[count].parent_email, row[8], sizeof(consents[count].parent_email) - 1);
        consents[count].parent_consent_type = row[9] ? (unsigned char)atoi(row[9]) : 0;
        consents[count].notice_type = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(consents[count].notice_addr, row[11], sizeof(consents[count].notice_addr) - 1);
        consents[count].create_date = row[12] ? (unsigned int)atoi(row[12]) : 0;
        consents[count].consent_date = row[13] ? (unsigned int)atoi(row[13]) : 0;
        consents[count].consent_yn = row[14] ? (unsigned char)atoi(row[14]) : 0;
        consents[count].history_yn = row[15] ? (unsigned char)atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_GetByParentEmail(DBConnectionManager* manager, const char* parent_email,
                                      UnderAgeConsent* consents, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !parent_email || !consents || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn "
        "FROM under_age_consent WHERE parent_email = '%s' ORDER BY create_date DESC",
        parent_email);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&consents[count], 0, sizeof(UnderAgeConsent));
        consents[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        consents[count].consent_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        consents[count].limit_money = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(consents[count].parent_name, row[3], sizeof(consents[count].parent_name) - 1);
        consents[count].parent_jumin = row[4] ? (unsigned long long)strtoull(row[4], NULL, 10) : 0;
        consents[count].parent_phone1 = row[5] ? (unsigned char)atoi(row[5]) : 0;
        consents[count].parent_phone2 = row[6] ? (unsigned short)atoi(row[6]) : 0;
        consents[count].parent_phone3 = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(consents[count].parent_email, row[8], sizeof(consents[count].parent_email) - 1);
        consents[count].parent_consent_type = row[9] ? (unsigned char)atoi(row[9]) : 0;
        consents[count].notice_type = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(consents[count].notice_addr, row[11], sizeof(consents[count].notice_addr) - 1);
        consents[count].create_date = row[12] ? (unsigned int)atoi(row[12]) : 0;
        consents[count].consent_date = row[13] ? (unsigned int)atoi(row[13]) : 0;
        consents[count].consent_yn = row[14] ? (unsigned char)atoi(row[14]) : 0;
        consents[count].history_yn = row[15] ? (unsigned char)atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_GetByConsentStatus(DBConnectionManager* manager, unsigned char consent_yn,
                                        UnderAgeConsent* consents, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !consents || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn "
        "FROM under_age_consent WHERE consent_yn = %u ORDER BY create_date DESC",
        consent_yn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&consents[count], 0, sizeof(UnderAgeConsent));
        consents[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        consents[count].consent_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        consents[count].limit_money = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(consents[count].parent_name, row[3], sizeof(consents[count].parent_name) - 1);
        consents[count].parent_jumin = row[4] ? (unsigned long long)strtoull(row[4], NULL, 10) : 0;
        consents[count].parent_phone1 = row[5] ? (unsigned char)atoi(row[5]) : 0;
        consents[count].parent_phone2 = row[6] ? (unsigned short)atoi(row[6]) : 0;
        consents[count].parent_phone3 = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(consents[count].parent_email, row[8], sizeof(consents[count].parent_email) - 1);
        consents[count].parent_consent_type = row[9] ? (unsigned char)atoi(row[9]) : 0;
        consents[count].notice_type = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(consents[count].notice_addr, row[11], sizeof(consents[count].notice_addr) - 1);
        consents[count].create_date = row[12] ? (unsigned int)atoi(row[12]) : 0;
        consents[count].consent_date = row[13] ? (unsigned int)atoi(row[13]) : 0;
        consents[count].consent_yn = row[14] ? (unsigned char)atoi(row[14]) : 0;
        consents[count].history_yn = row[15] ? (unsigned char)atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_GetAll(DBConnectionManager* manager, UnderAgeConsent* consents,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !consents || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, "
        "parent_phone1, parent_phone2, parent_phone3, parent_email, "
        "parent_consent_type, notice_type, notice_addr, create_date, "
        "consent_date, consent_yn, history_yn "
        "FROM under_age_consent ORDER BY create_date DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&consents[count], 0, sizeof(UnderAgeConsent));
        consents[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        consents[count].consent_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        consents[count].limit_money = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(consents[count].parent_name, row[3], sizeof(consents[count].parent_name) - 1);
        consents[count].parent_jumin = row[4] ? (unsigned long long)strtoull(row[4], NULL, 10) : 0;
        consents[count].parent_phone1 = row[5] ? (unsigned char)atoi(row[5]) : 0;
        consents[count].parent_phone2 = row[6] ? (unsigned short)atoi(row[6]) : 0;
        consents[count].parent_phone3 = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(consents[count].parent_email, row[8], sizeof(consents[count].parent_email) - 1);
        consents[count].parent_consent_type = row[9] ? (unsigned char)atoi(row[9]) : 0;
        consents[count].notice_type = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(consents[count].notice_addr, row[11], sizeof(consents[count].notice_addr) - 1);
        consents[count].create_date = row[12] ? (unsigned int)atoi(row[12]) : 0;
        consents[count].consent_date = row[13] ? (unsigned int)atoi(row[13]) : 0;
        consents[count].consent_yn = row[14] ? (unsigned char)atoi(row[14]) : 0;
        consents[count].history_yn = row[15] ? (unsigned char)atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void UnderAgeConsent_PrintInfo(const UnderAgeConsent* consent) {
    if (!consent) return;

    printf("=== Under Age Consent Info ===\n");
    printf("Member ID: %u\n", consent->m_id);
    printf("Consent Type: %u\n", consent->consent_type);
    printf("Limit Money: %u\n", consent->limit_money);
    printf("Parent Name: %s\n", consent->parent_name);
    printf("Parent Jumin: %llu\n", consent->parent_jumin);
    printf("Parent Phone: %03u-%04u-%04u\n",
           consent->parent_phone1, consent->parent_phone2, consent->parent_phone3);
    printf("Parent Email: %s\n", consent->parent_email);
    printf("Parent Consent Type: %u\n", consent->parent_consent_type);
    printf("Notice Type: %u\n", consent->notice_type);
    printf("Notice Address: %s\n", consent->notice_addr);
    printf("Create Date: %u\n", consent->create_date);
    printf("Consent Date: %u\n", consent->consent_date);
    printf("Consent YN: %u\n", consent->consent_yn);
    printf("History YN: %u\n", consent->history_yn);
    printf("==============================\n");
}
