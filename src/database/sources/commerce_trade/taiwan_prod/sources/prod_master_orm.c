#include "prod_master_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdMaster_Add(DBConnectionManager* manager, const ProdMaster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_master (prod_part_id, prod_part_script, prod_part_name) "
        "VALUES (%d, '%s', '%s')",
        record->prod_part_id, record->prod_part_script, record->prod_part_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdMaster_Get(DBConnectionManager* manager, short prod_part_id, ProdMaster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT prod_part_id, prod_part_script, prod_part_name FROM prod_master WHERE prod_part_id = %d",
        prod_part_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdMaster));
    record->prod_part_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->prod_part_script, row[1], sizeof(record->prod_part_script) - 1);
    if (row[2]) strncpy(record->prod_part_name, row[2], sizeof(record->prod_part_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ProdMaster_GetAll(DBConnectionManager* manager, ProdMaster* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT prod_part_id, prod_part_script, prod_part_name FROM prod_master");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdMaster));
        records[count].prod_part_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].prod_part_script, row[1], sizeof(records[count].prod_part_script) - 1);
        if (row[2]) strncpy(records[count].prod_part_name, row[2], sizeof(records[count].prod_part_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
