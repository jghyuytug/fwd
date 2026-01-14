#include "dnf_charac_mov_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfCharacMov_Add(DBConnectionManager* manager, const DnfCharacMov* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_charac_mov (m_id, server_id, charac_no, move_server_id, move_charac_no, move_check) "
        "VALUES (%d, %d, %d, %d, %d, %d)",
        record->m_id, record->server_id, record->charac_no, record->move_server_id, record->move_charac_no, record->move_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_Get(DBConnectionManager* manager, int id, DnfCharacMov* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check FROM dnf_charac_mov WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfCharacMov));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->move_server_id = row[4] ? atoi(row[4]) : 0;
    record->move_charac_no = row[5] ? atoi(row[5]) : 0;
    record->move_check = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_GetAll(DBConnectionManager* manager, DnfCharacMov* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check FROM dnf_charac_mov");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfCharacMov));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].move_server_id = row[4] ? atoi(row[4]) : 0;
        records[count].move_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].move_check = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
