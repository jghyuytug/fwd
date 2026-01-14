#include "prod_sale_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdSaleDaily_Add(DBConnectionManager* manager, const ProdSaleDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_sale_daily (sale_date, slot_part_code, ipg_no, volume, amount, un_m_id, un_charac) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d)",
        record->sale_date, record->slot_part_code, record->ipg_no, record->volume, record->amount, record->un_m_id, record->un_charac);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSaleDaily_Get(DBConnectionManager* manager, const char* sale_date, int ipg_no, ProdSaleDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sale_date, slot_part_code, ipg_no, volume, amount, un_m_id, un_charac FROM prod_sale_daily WHERE sale_date = '%s' AND ipg_no = %d",
        sale_date, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdSaleDaily));
    if (row[0]) strncpy(record->sale_date, row[0], sizeof(record->sale_date) - 1);
    record->slot_part_code = row[1] ? atoi(row[1]) : 0;
    record->ipg_no = row[2] ? atoi(row[2]) : 0;
    record->volume = row[3] ? atoi(row[3]) : 0;
    record->amount = row[4] ? atoll(row[4]) : 0;
    record->un_m_id = row[5] ? atoi(row[5]) : 0;
    record->un_charac = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSaleDaily_GetAll(DBConnectionManager* manager, ProdSaleDaily* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sale_date, slot_part_code, ipg_no, volume, amount, un_m_id, un_charac FROM prod_sale_daily");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdSaleDaily));
        if (row[0]) strncpy(records[count].sale_date, row[0], sizeof(records[count].sale_date) - 1);
        records[count].slot_part_code = row[1] ? atoi(row[1]) : 0;
        records[count].ipg_no = row[2] ? atoi(row[2]) : 0;
        records[count].volume = row[3] ? atoi(row[3]) : 0;
        records[count].amount = row[4] ? atoll(row[4]) : 0;
        records[count].un_m_id = row[5] ? atoi(row[5]) : 0;
        records[count].un_charac = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
