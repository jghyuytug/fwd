#include "dnf_restrict_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfRestrictInfo_Add(DBConnectionManager* manager, const DnfRestrictInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_restrict_info (category, restrict_code, restrict_str, reg_date) "
        "VALUES (%d, %d, '%s', '%s')",
        record->category, record->restrict_code, record->restrict_str, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_Get(DBConnectionManager* manager, int category, int restrict_code, DnfRestrictInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT category, restrict_code, restrict_str, reg_date FROM dnf_restrict_info WHERE category = %d AND restrict_code = %d",
        category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfRestrictInfo));
    record->category = row[0] ? atoi(row[0]) : 0;
    record->restrict_code = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->restrict_str, row[2], sizeof(record->restrict_str) - 1);
    if (row[3]) strncpy(record->reg_date, row[3], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_GetAll(DBConnectionManager* manager, DnfRestrictInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT category, restrict_code, restrict_str, reg_date FROM dnf_restrict_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfRestrictInfo));
        records[count].category = row[0] ? atoi(row[0]) : 0;
        records[count].restrict_code = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].restrict_str, row[2], sizeof(records[count].restrict_str) - 1);
        if (row[3]) strncpy(records[count].reg_date, row[3], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
