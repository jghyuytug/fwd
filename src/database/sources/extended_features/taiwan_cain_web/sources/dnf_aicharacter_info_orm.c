#include "dnf_aicharacter_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfAicharacterInfo_Add(DBConnectionManager* manager, const DnfAicharacterInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_aicharacter_info (idx, ai_name_kr) "
        "VALUES (%d, '%s')",
        record->idx, record->ai_name_kr);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfAicharacterInfo_GetAll(DBConnectionManager* manager, DnfAicharacterInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, ai_name_kr FROM dnf_aicharacter_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfAicharacterInfo));
        records[count].idx = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].ai_name_kr, row[1], sizeof(records[count].ai_name_kr) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
