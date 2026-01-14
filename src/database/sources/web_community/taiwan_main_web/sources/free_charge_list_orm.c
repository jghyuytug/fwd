#include "free_charge_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int FreeChargeList_Add(DBConnectionManager* manager, const FreeChargeList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO free_charge_list (m_id, product_cd, sera_amount, item_cd, reg_date) "
        "VALUES (%d, %d, %d, %d, '%s')",
        record->m_id, record->product_cd, record->sera_amount, record->item_cd, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int FreeChargeList_Get(DBConnectionManager* manager, int tid, FreeChargeList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT tid, m_id, product_cd, sera_amount, item_cd, reg_date FROM free_charge_list WHERE tid = %d",
        tid);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(FreeChargeList));
    record->tid = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->product_cd = row[2] ? atoi(row[2]) : 0;
    record->sera_amount = row[3] ? atoi(row[3]) : 0;
    record->item_cd = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->reg_date, row[5], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int FreeChargeList_GetAll(DBConnectionManager* manager, FreeChargeList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tid, m_id, product_cd, sera_amount, item_cd, reg_date FROM free_charge_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(FreeChargeList));
        records[count].tid = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].product_cd = row[2] ? atoi(row[2]) : 0;
        records[count].sera_amount = row[3] ? atoi(row[3]) : 0;
        records[count].item_cd = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].reg_date, row[5], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
