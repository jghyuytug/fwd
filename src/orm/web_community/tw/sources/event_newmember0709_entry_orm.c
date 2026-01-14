#include "event_newmember0709_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventNewmember0709Entry_Add(DBConnectionManager* manager, const EventNewmember0709Entry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_newmember0709_entry (m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
        record->m_id, record->occ_date, record->server_id, record->charac_no, record->item1_no, record->item1_check, record->item2_no, record->item2_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_Get(DBConnectionManager* manager, int m_id, EventNewmember0709Entry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check FROM event_newmember0709_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventNewmember0709Entry));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->occ_date = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->item1_no = row[4] ? atoi(row[4]) : 0;
    record->item1_check = row[5] ? atoi(row[5]) : 0;
    record->item2_no = row[6] ? atoi(row[6]) : 0;
    record->item2_check = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_GetAll(DBConnectionManager* manager, EventNewmember0709Entry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check FROM event_newmember0709_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventNewmember0709Entry));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_date = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].item1_no = row[4] ? atoi(row[4]) : 0;
        records[count].item1_check = row[5] ? atoi(row[5]) : 0;
        records[count].item2_no = row[6] ? atoi(row[6]) : 0;
        records[count].item2_check = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
