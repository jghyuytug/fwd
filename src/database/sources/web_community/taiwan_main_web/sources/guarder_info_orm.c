#include "guarder_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuarderInfo_Add(DBConnectionManager* manager, const GuarderInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guarder_info (m_id, g_name, g_birth, g_phone, g_confirm) "
        "VALUES (%d, '%s', '%s', '%s', %d)",
        record->m_id, record->g_name, record->g_birth, record->g_phone, record->g_confirm);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuarderInfo_Get(DBConnectionManager* manager, int m_id, GuarderInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, g_name, g_birth, g_phone, g_confirm FROM guarder_info WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuarderInfo));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->g_name, row[1], sizeof(record->g_name) - 1);
    if (row[2]) strncpy(record->g_birth, row[2], sizeof(record->g_birth) - 1);
    if (row[3]) strncpy(record->g_phone, row[3], sizeof(record->g_phone) - 1);
    record->g_confirm = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuarderInfo_GetAll(DBConnectionManager* manager, GuarderInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, g_name, g_birth, g_phone, g_confirm FROM guarder_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuarderInfo));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].g_name, row[1], sizeof(records[count].g_name) - 1);
        if (row[2]) strncpy(records[count].g_birth, row[2], sizeof(records[count].g_birth) - 1);
        if (row[3]) strncpy(records[count].g_phone, row[3], sizeof(records[count].g_phone) - 1);
        records[count].g_confirm = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
