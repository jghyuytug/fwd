#include "dnf_event_address_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfEventAddress_Add(DBConnectionManager* manager, const DnfEventAddress* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_event_address (event_id, m_id, occ_date, zipcode, address, phone_no) "
        "VALUES (%d, %d, '%s', '%s', '%s', '%s')",
        record->event_id, record->m_id, record->occ_date, record->zipcode, record->address, record->phone_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_Get(DBConnectionManager* manager, int event_id, int m_id, DnfEventAddress* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no FROM dnf_event_address WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfEventAddress));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_date, row[2], sizeof(record->occ_date) - 1);
    if (row[3]) strncpy(record->zipcode, row[3], sizeof(record->zipcode) - 1);
    if (row[4]) strncpy(record->address, row[4], sizeof(record->address) - 1);
    if (row[5]) strncpy(record->phone_no, row[5], sizeof(record->phone_no) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_GetAll(DBConnectionManager* manager, DnfEventAddress* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no FROM dnf_event_address");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfEventAddress));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_date, row[2], sizeof(records[count].occ_date) - 1);
        if (row[3]) strncpy(records[count].zipcode, row[3], sizeof(records[count].zipcode) - 1);
        if (row[4]) strncpy(records[count].address, row[4], sizeof(records[count].address) - 1);
        if (row[5]) strncpy(records[count].phone_no, row[5], sizeof(records[count].phone_no) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
