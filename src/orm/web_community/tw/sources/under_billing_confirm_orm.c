#include "under_billing_confirm_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UnderBillingConfirm_Add(DBConnectionManager* manager, const UnderBillingConfirm* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO under_billing_confirm (m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, parent_email, parent_consent_type, create_date, consent_date, consent_yn) "
        "VALUES (%d, '%s', %d, %d, %d, %d, '%s', %d, %d, %d, %d)",
        record->m_id, record->parent_name, record->parent_jumin, record->parent_phone1, record->parent_phone2, record->parent_phone3, record->parent_email, record->parent_consent_type, record->create_date, record->consent_date, record->consent_yn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_Get(DBConnectionManager* manager, int m_id, UnderBillingConfirm* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, parent_email, parent_consent_type, create_date, consent_date, consent_yn FROM under_billing_confirm WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(UnderBillingConfirm));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->parent_name, row[1], sizeof(record->parent_name) - 1);
    record->parent_jumin = row[2] ? atoll(row[2]) : 0;
    record->parent_phone1 = row[3] ? atoi(row[3]) : 0;
    record->parent_phone2 = row[4] ? atoi(row[4]) : 0;
    record->parent_phone3 = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->parent_email, row[6], sizeof(record->parent_email) - 1);
    record->parent_consent_type = row[7] ? atoi(row[7]) : 0;
    record->create_date = row[8] ? atoi(row[8]) : 0;
    record->consent_date = row[9] ? atoi(row[9]) : 0;
    record->consent_yn = row[10] ? atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int UnderBillingConfirm_GetAll(DBConnectionManager* manager, UnderBillingConfirm* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, parent_email, parent_consent_type, create_date, consent_date, consent_yn FROM under_billing_confirm");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UnderBillingConfirm));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].parent_name, row[1], sizeof(records[count].parent_name) - 1);
        records[count].parent_jumin = row[2] ? atoll(row[2]) : 0;
        records[count].parent_phone1 = row[3] ? atoi(row[3]) : 0;
        records[count].parent_phone2 = row[4] ? atoi(row[4]) : 0;
        records[count].parent_phone3 = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].parent_email, row[6], sizeof(records[count].parent_email) - 1);
        records[count].parent_consent_type = row[7] ? atoi(row[7]) : 0;
        records[count].create_date = row[8] ? atoi(row[8]) : 0;
        records[count].consent_date = row[9] ? atoi(row[9]) : 0;
        records[count].consent_yn = row[10] ? atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
