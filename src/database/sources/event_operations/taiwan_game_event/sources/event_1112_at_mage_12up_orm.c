#include "event_1112_at_mage_12up_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Event1112AtMage12Up_Add(DBConnectionManager* manager, const Event1112AtMage12Up* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_1112_at_mage_12up (m_id, charac_no, server_id, occ_date) "
        "VALUES (%d, %d, %d, '%s')",
        record->m_id, record->charac_no, record->server_id, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1112AtMage12Up_Get(DBConnectionManager* manager, int m_id, Event1112AtMage12Up* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, server_id, occ_date FROM event_1112_at_mage_12up WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Event1112AtMage12Up));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_date, row[3], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Event1112AtMage12Up_GetAll(DBConnectionManager* manager, Event1112AtMage12Up* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, server_id, occ_date FROM event_1112_at_mage_12up");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Event1112AtMage12Up));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_date, row[3], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
