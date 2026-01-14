#include "penpic_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PenpicEntry_Add(DBConnectionManager* manager, const PenpicEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO penpic_entry (m_id, penpic_type, cnt) "
        "VALUES (%d, %d, %d)",
        record->m_id, record->penpic_type, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PenpicEntry_Get(DBConnectionManager* manager, int m_id, signed char penpic_type, PenpicEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, penpic_type, cnt FROM penpic_entry WHERE m_id = %d AND penpic_type = %d",
        m_id, penpic_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PenpicEntry));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->penpic_type = row[1] ? atoi(row[1]) : 0;
    record->cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PenpicEntry_GetAll(DBConnectionManager* manager, PenpicEntry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, penpic_type, cnt FROM penpic_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PenpicEntry));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].penpic_type = row[1] ? atoi(row[1]) : 0;
        records[count].cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
