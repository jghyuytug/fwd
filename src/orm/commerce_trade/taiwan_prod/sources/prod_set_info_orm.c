#include "prod_set_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdSetInfo_Add(DBConnectionManager* manager, const ProdSetInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_set_info (ipg_no, consist_ipg_no, fix_flag) "
        "VALUES (%d, %d, %d)",
        record->ipg_no, record->consist_ipg_no, record->fix_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSetInfo_Get(DBConnectionManager* manager, int ipg_no, int consist_ipg_no, ProdSetInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ipg_no, consist_ipg_no, fix_flag FROM prod_set_info WHERE ipg_no = %d AND consist_ipg_no = %d",
        ipg_no, consist_ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdSetInfo));
    record->ipg_no = row[0] ? atoi(row[0]) : 0;
    record->consist_ipg_no = row[1] ? atoi(row[1]) : 0;
    record->fix_flag = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSetInfo_GetAll(DBConnectionManager* manager, ProdSetInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ipg_no, consist_ipg_no, fix_flag FROM prod_set_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdSetInfo));
        records[count].ipg_no = row[0] ? atoi(row[0]) : 0;
        records[count].consist_ipg_no = row[1] ? atoi(row[1]) : 0;
        records[count].fix_flag = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
