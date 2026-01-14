#include "prod_sale_daily_detail_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdSaleDailyDetail_Add(DBConnectionManager* manager, const ProdSaleDailyDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_sale_daily_detail (sale_date, slot_part_code, ipg_no, sex, age, job, volume, amount) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d)",
        record->sale_date, record->slot_part_code, record->ipg_no, record->sex, record->age, record->job, record->volume, record->amount);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSaleDailyDetail_Get(DBConnectionManager* manager, const char* sale_date, int ipg_no, char sex, signed char age, signed char job, ProdSaleDailyDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sale_date, slot_part_code, ipg_no, sex, age, job, volume, amount FROM prod_sale_daily_detail WHERE sale_date = '%s' AND ipg_no = %d AND sex = %d AND age = %d AND job = %d",
        sale_date, ipg_no, sex, age, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdSaleDailyDetail));
    if (row[0]) strncpy(record->sale_date, row[0], sizeof(record->sale_date) - 1);
    record->slot_part_code = row[1] ? atoi(row[1]) : 0;
    record->ipg_no = row[2] ? atoi(row[2]) : 0;
    record->sex = row[3] ? row[3][0] : '\0';
    record->age = row[4] ? atoi(row[4]) : 0;
    record->job = row[5] ? atoi(row[5]) : 0;
    record->volume = row[6] ? atoi(row[6]) : 0;
    record->amount = row[7] ? atoll(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSaleDailyDetail_GetAll(DBConnectionManager* manager, ProdSaleDailyDetail* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sale_date, slot_part_code, ipg_no, sex, age, job, volume, amount FROM prod_sale_daily_detail");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdSaleDailyDetail));
        if (row[0]) strncpy(records[count].sale_date, row[0], sizeof(records[count].sale_date) - 1);
        records[count].slot_part_code = row[1] ? atoi(row[1]) : 0;
        records[count].ipg_no = row[2] ? atoi(row[2]) : 0;
        records[count].sex = row[3] ? row[3][0] : '\0';
        records[count].age = row[4] ? atoi(row[4]) : 0;
        records[count].job = row[5] ? atoi(row[5]) : 0;
        records[count].volume = row[6] ? atoi(row[6]) : 0;
        records[count].amount = row[7] ? atoll(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
