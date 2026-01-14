#include "id_pass_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int IdPass_Add(DBConnectionManager* manager, const IdPass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO id_pass (m_id, user_id, name, phone, code, tid, update_day, create_day) "
        "VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s', '%s')",
        record->m_id, record->user_id, record->name, record->phone, record->code, record->tid, record->update_day, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int IdPass_Get(DBConnectionManager* manager, int no, IdPass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, user_id, name, phone, code, tid, update_day, create_day FROM id_pass WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(IdPass));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->user_id, row[2], sizeof(record->user_id) - 1);
    if (row[3]) strncpy(record->name, row[3], sizeof(record->name) - 1);
    if (row[4]) strncpy(record->phone, row[4], sizeof(record->phone) - 1);
    record->code = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->tid, row[6], sizeof(record->tid) - 1);
    if (row[7]) strncpy(record->update_day, row[7], sizeof(record->update_day) - 1);
    if (row[8]) strncpy(record->create_day, row[8], sizeof(record->create_day) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int IdPass_GetAll(DBConnectionManager* manager, IdPass* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, user_id, name, phone, code, tid, update_day, create_day FROM id_pass");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(IdPass));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].user_id, row[2], sizeof(records[count].user_id) - 1);
        if (row[3]) strncpy(records[count].name, row[3], sizeof(records[count].name) - 1);
        if (row[4]) strncpy(records[count].phone, row[4], sizeof(records[count].phone) - 1);
        records[count].code = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].tid, row[6], sizeof(records[count].tid) - 1);
        if (row[7]) strncpy(records[count].update_day, row[7], sizeof(records[count].update_day) - 1);
        if (row[8]) strncpy(records[count].create_day, row[8], sizeof(records[count].create_day) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
