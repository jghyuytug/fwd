#include "event_goldcard_entry2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventGoldcardEntry2_Add(DBConnectionManager* manager, const EventGoldcardEntry2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_goldcard_entry2 (occ_date, m_id, server_id, charac_no, item_no, item_check) "
        "VALUES (%d, %d, %d, %d, %d, %d)",
        record->occ_date, record->m_id, record->server_id, record->charac_no, record->item_no, record->item_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_Get(DBConnectionManager* manager, int occ_date, int m_id, EventGoldcardEntry2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, server_id, charac_no, item_no, item_check FROM event_goldcard_entry2 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventGoldcardEntry2));
    record->occ_date = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->item_no = row[4] ? atoi(row[4]) : 0;
    record->item_check = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_GetAll(DBConnectionManager* manager, EventGoldcardEntry2* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, server_id, charac_no, item_no, item_check FROM event_goldcard_entry2");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventGoldcardEntry2));
        records[count].occ_date = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].item_no = row[4] ? atoi(row[4]) : 0;
        records[count].item_check = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
