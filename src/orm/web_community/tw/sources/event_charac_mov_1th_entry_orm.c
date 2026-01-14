#include "event_charac_mov_1th_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventCharacMov1ThEntry_Add(DBConnectionManager* manager, const EventCharacMov1ThEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_charac_mov_1th_entry (m_id, occ_time, it_no, item_check) "
        "VALUES (%d, %d, %d, %d)",
        record->m_id, record->occ_time, record->it_no, record->item_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1ThEntry_Get(DBConnectionManager* manager, int m_id, EventCharacMov1ThEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, it_no, item_check FROM event_charac_mov_1th_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventCharacMov1ThEntry));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->it_no = row[2] ? atoi(row[2]) : 0;
    record->item_check = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1ThEntry_GetAll(DBConnectionManager* manager, EventCharacMov1ThEntry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, it_no, item_check FROM event_charac_mov_1th_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventCharacMov1ThEntry));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].it_no = row[2] ? atoi(row[2]) : 0;
        records[count].item_check = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
