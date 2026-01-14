#include "dnf_item_info_master_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfItemInfoMaster_Add(DBConnectionManager* manager, const DnfItemInfoMaster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_item_info_master (master_no, sub_no, name, master_explain) "
        "VALUES (%d, %d, '%s', '%s')",
        record->master_no, record->sub_no, record->name, record->master_explain);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfoMaster_Get(DBConnectionManager* manager, signed char master_no, short sub_no, DnfItemInfoMaster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT master_no, sub_no, name, master_explain FROM dnf_item_info_master WHERE master_no = %d AND sub_no = %d",
        master_no, sub_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfItemInfoMaster));
    record->master_no = row[0] ? atoi(row[0]) : 0;
    record->sub_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->name, row[2], sizeof(record->name) - 1);
    if (row[3]) strncpy(record->master_explain, row[3], sizeof(record->master_explain) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfoMaster_GetAll(DBConnectionManager* manager, DnfItemInfoMaster* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT master_no, sub_no, name, master_explain FROM dnf_item_info_master");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfItemInfoMaster));
        records[count].master_no = row[0] ? atoi(row[0]) : 0;
        records[count].sub_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].name, row[2], sizeof(records[count].name) - 1);
        if (row[3]) strncpy(records[count].master_explain, row[3], sizeof(records[count].master_explain) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
