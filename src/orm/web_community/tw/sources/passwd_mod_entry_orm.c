#include "passwd_mod_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PasswdModEntry_Add(DBConnectionManager* manager, const PasswdModEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO passwd_mod_entry (m_id, occ_time, ip, pre_passwd) "
        "VALUES (%d, '%s', '%s', '%s')",
        record->m_id, record->occ_time, record->ip, record->pre_passwd);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_Get(DBConnectionManager* manager, int m_id, const char* occ_time, PasswdModEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry WHERE m_id = %d AND occ_time = '%s'",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PasswdModEntry));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    if (row[2]) strncpy(record->ip, row[2], sizeof(record->ip) - 1);
    if (row[3]) strncpy(record->pre_passwd, row[3], sizeof(record->pre_passwd) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_GetAll(DBConnectionManager* manager, PasswdModEntry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PasswdModEntry));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        if (row[2]) strncpy(records[count].ip, row[2], sizeof(records[count].ip) - 1);
        if (row[3]) strncpy(records[count].pre_passwd, row[3], sizeof(records[count].pre_passwd) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
