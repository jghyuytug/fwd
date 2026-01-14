#include "member_info_detail_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberInfoDetail_Add(DBConnectionManager* manager, const MemberInfoDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_info_detail (m_id, zipcode, address, address_detail, occ_date) "
        "VALUES (%d, '%s', '%s', '%s', '%s')",
        record->m_id, record->zipcode, record->address, record->address_detail, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoDetail_Get(DBConnectionManager* manager, int m_id, MemberInfoDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date FROM member_info_detail WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberInfoDetail));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->zipcode, row[1], sizeof(record->zipcode) - 1);
    if (row[2]) strncpy(record->address, row[2], sizeof(record->address) - 1);
    if (row[3]) strncpy(record->address_detail, row[3], sizeof(record->address_detail) - 1);
    if (row[4]) strncpy(record->occ_date, row[4], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoDetail_GetAll(DBConnectionManager* manager, MemberInfoDetail* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date FROM member_info_detail");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberInfoDetail));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].zipcode, row[1], sizeof(records[count].zipcode) - 1);
        if (row[2]) strncpy(records[count].address, row[2], sizeof(records[count].address) - 1);
        if (row[3]) strncpy(records[count].address_detail, row[3], sizeof(records[count].address_detail) - 1);
        if (row[4]) strncpy(records[count].occ_date, row[4], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
