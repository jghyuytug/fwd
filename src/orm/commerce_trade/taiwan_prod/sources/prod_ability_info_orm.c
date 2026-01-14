#include "prod_ability_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdAbilityInfo_Add(DBConnectionManager* manager, const ProdAbilityInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_ability_info (ipg_no, it_no, job, grow_type, ability_no, rate) "
        "VALUES (%d, %d, %d, %d, %d, %d)",
        record->ipg_no, record->it_no, record->job, record->grow_type, record->ability_no, record->rate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdAbilityInfo_Get(DBConnectionManager* manager, int ipg_no, signed char job, signed char grow_type, int ability_no, ProdAbilityInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ipg_no, it_no, job, grow_type, ability_no, rate FROM prod_ability_info WHERE ipg_no = %d AND job = %d AND grow_type = %d AND ability_no = %d",
        ipg_no, job, grow_type, ability_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdAbilityInfo));
    record->ipg_no = row[0] ? atoi(row[0]) : 0;
    record->it_no = row[1] ? atoi(row[1]) : 0;
    record->job = row[2] ? atoi(row[2]) : 0;
    record->grow_type = row[3] ? atoi(row[3]) : 0;
    record->ability_no = row[4] ? atoi(row[4]) : 0;
    record->rate = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ProdAbilityInfo_GetAll(DBConnectionManager* manager, ProdAbilityInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ipg_no, it_no, job, grow_type, ability_no, rate FROM prod_ability_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdAbilityInfo));
        records[count].ipg_no = row[0] ? atoi(row[0]) : 0;
        records[count].it_no = row[1] ? atoi(row[1]) : 0;
        records[count].job = row[2] ? atoi(row[2]) : 0;
        records[count].grow_type = row[3] ? atoi(row[3]) : 0;
        records[count].ability_no = row[4] ? atoi(row[4]) : 0;
        records[count].rate = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
