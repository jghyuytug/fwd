#include "dnf_item_price_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfItemPrice_Add(DBConnectionManager* manager, const DnfItemPrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_item_price (it_no, it_cnt, cera_price, ipg_no) "
        "VALUES (%d, %d, %d, %d)",
        record->it_no, record->it_cnt, record->cera_price, record->ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemPrice_Get(DBConnectionManager* manager, int ipg_no, DnfItemPrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, it_cnt, cera_price, ipg_no FROM dnf_item_price WHERE ipg_no = %d",
        ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfItemPrice));
    record->it_no = row[0] ? atoi(row[0]) : 0;
    record->it_cnt = row[1] ? atoi(row[1]) : 0;
    record->cera_price = row[2] ? atoi(row[2]) : 0;
    record->ipg_no = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemPrice_GetAll(DBConnectionManager* manager, DnfItemPrice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, it_cnt, cera_price, ipg_no FROM dnf_item_price");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfItemPrice));
        records[count].it_no = row[0] ? atoi(row[0]) : 0;
        records[count].it_cnt = row[1] ? atoi(row[1]) : 0;
        records[count].cera_price = row[2] ? atoi(row[2]) : 0;
        records[count].ipg_no = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
