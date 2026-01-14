#include "free_charge_product_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int FreeChargeProductList_Add(DBConnectionManager* manager, const FreeChargeProductList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO free_charge_product_list (product_name, sera_amount, item_cd, reg_date) "
        "VALUES ('%s', %d, %d, '%s')",
        record->product_name, record->sera_amount, record->item_cd, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int FreeChargeProductList_Get(DBConnectionManager* manager, int product_cd, FreeChargeProductList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT product_cd, product_name, sera_amount, item_cd, reg_date FROM free_charge_product_list WHERE product_cd = %d",
        product_cd);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(FreeChargeProductList));
    record->product_cd = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->product_name, row[1], sizeof(record->product_name) - 1);
    record->sera_amount = row[2] ? atoi(row[2]) : 0;
    record->item_cd = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->reg_date, row[4], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int FreeChargeProductList_GetAll(DBConnectionManager* manager, FreeChargeProductList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT product_cd, product_name, sera_amount, item_cd, reg_date FROM free_charge_product_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(FreeChargeProductList));
        records[count].product_cd = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].product_name, row[1], sizeof(records[count].product_name) - 1);
        records[count].sera_amount = row[2] ? atoi(row[2]) : 0;
        records[count].item_cd = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].reg_date, row[4], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
