#include "dnf_event_entry_notuse_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfEventEntryNotuse_Add(DBConnectionManager* manager, const DnfEventEntryNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_event_entry_notuse (event_id, m_id, occ_date, server_id, charac_no, obtain_date) "
        "VALUES (%d, %d, '%s', %d, %d, '%s')",
        record->event_id, record->m_id, record->occ_date, record->server_id, record->charac_no, record->obtain_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntryNotuse_Get(DBConnectionManager* manager, int event_id, int m_id, DnfEventEntryNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date FROM dnf_event_entry_notuse WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfEventEntryNotuse));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_date, row[2], sizeof(record->occ_date) - 1);
    record->server_id = row[3] ? atoi(row[3]) : 0;
    record->charac_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->obtain_date, row[5], sizeof(record->obtain_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntryNotuse_GetAll(DBConnectionManager* manager, DnfEventEntryNotuse* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date FROM dnf_event_entry_notuse");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfEventEntryNotuse));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_date, row[2], sizeof(records[count].occ_date) - 1);
        records[count].server_id = row[3] ? atoi(row[3]) : 0;
        records[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].obtain_date, row[5], sizeof(records[count].obtain_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
