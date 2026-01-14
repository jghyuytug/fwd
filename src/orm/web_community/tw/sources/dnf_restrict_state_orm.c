#include "dnf_restrict_state_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfRestrictState_Add(DBConnectionManager* manager, const DnfRestrictState* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_restrict_state (server_group, category, restrict_code, restrict_value, mod_date, reg_date) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s')",
        record->server_group, record->category, record->restrict_code, record->restrict_value, record->mod_date, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_Get(DBConnectionManager* manager, int server_group, int category, int restrict_code, DnfRestrictState* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, category, restrict_code, restrict_value, mod_date, reg_date FROM dnf_restrict_state WHERE server_group = %d AND category = %d AND restrict_code = %d",
        server_group, category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfRestrictState));
    record->server_group = row[0] ? atoi(row[0]) : 0;
    record->category = row[1] ? atoi(row[1]) : 0;
    record->restrict_code = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->restrict_value, row[3], sizeof(record->restrict_value) - 1);
    if (row[4]) strncpy(record->mod_date, row[4], sizeof(record->mod_date) - 1);
    if (row[5]) strncpy(record->reg_date, row[5], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_GetAll(DBConnectionManager* manager, DnfRestrictState* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, category, restrict_code, restrict_value, mod_date, reg_date FROM dnf_restrict_state");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfRestrictState));
        records[count].server_group = row[0] ? atoi(row[0]) : 0;
        records[count].category = row[1] ? atoi(row[1]) : 0;
        records[count].restrict_code = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].restrict_value, row[3], sizeof(records[count].restrict_value) - 1);
        if (row[4]) strncpy(records[count].mod_date, row[4], sizeof(records[count].mod_date) - 1);
        if (row[5]) strncpy(records[count].reg_date, row[5], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
