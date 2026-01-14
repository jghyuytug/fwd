#include "member_mac_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberMacInfo_Add(DBConnectionManager* manager, const MemberMacInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_mac_info (mac_addr, occ_time) "
        "VALUES ('%s', '%s')",
        record->mac_addr, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMacInfo_Get(DBConnectionManager* manager, int no, MemberMacInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, mac_addr, occ_time FROM member_mac_info WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberMacInfo));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->mac_addr, row[1], sizeof(record->mac_addr) - 1);
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMacInfo_GetAll(DBConnectionManager* manager, MemberMacInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, mac_addr, occ_time FROM member_mac_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMacInfo));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].mac_addr, row[1], sizeof(records[count].mac_addr) - 1);
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
