#include "under_age_consent_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UnderAgeConsent_Add(DBConnectionManager* manager, const UnderAgeConsent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO under_age_consent (m_id, consent_type, limit_money, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, parent_email, parent_consent_type, notice_type, notice_addr, create_date, consent_date, consent_yn, history_yn) "
        "VALUES (%d, %d, %d, '%s', %d, %d, %d, %d, '%s', %d, %d, '%s', %d, %d, %d, %d)",
        record->m_id, record->consent_type, record->limit_money, record->parent_name, record->parent_jumin, record->parent_phone1, record->parent_phone2, record->parent_phone3, record->parent_email, record->parent_consent_type, record->notice_type, record->notice_addr, record->create_date, record->consent_date, record->consent_yn, record->history_yn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UnderAgeConsent_GetAll(DBConnectionManager* manager, UnderAgeConsent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, consent_type, limit_money, parent_name, parent_jumin, parent_phone1, parent_phone2, parent_phone3, parent_email, parent_consent_type, notice_type, notice_addr, create_date, consent_date, consent_yn, history_yn FROM under_age_consent");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UnderAgeConsent));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].consent_type = row[1] ? atoi(row[1]) : 0;
        records[count].limit_money = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].parent_name, row[3], sizeof(records[count].parent_name) - 1);
        records[count].parent_jumin = row[4] ? atoll(row[4]) : 0;
        records[count].parent_phone1 = row[5] ? atoi(row[5]) : 0;
        records[count].parent_phone2 = row[6] ? atoi(row[6]) : 0;
        records[count].parent_phone3 = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].parent_email, row[8], sizeof(records[count].parent_email) - 1);
        records[count].parent_consent_type = row[9] ? atoi(row[9]) : 0;
        records[count].notice_type = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].notice_addr, row[11], sizeof(records[count].notice_addr) - 1);
        records[count].create_date = row[12] ? atoi(row[12]) : 0;
        records[count].consent_date = row[13] ? atoi(row[13]) : 0;
        records[count].consent_yn = row[14] ? atoi(row[14]) : 0;
        records[count].history_yn = row[15] ? atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
