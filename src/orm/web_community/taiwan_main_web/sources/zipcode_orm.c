#include "zipcode_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Zipcode_Add(DBConnectionManager* manager, const Zipcode* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO zipcode (zipcode, sido, gugun, dong, bunji) "
        "VALUES ('%s', '%s', '%s', '%s', '%s')",
        record->zipcode, record->sido, record->gugun, record->dong, record->bunji);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Zipcode_GetAll(DBConnectionManager* manager, Zipcode* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT zipcode, sido, gugun, dong, bunji FROM zipcode");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Zipcode));
        if (row[0]) strncpy(records[count].zipcode, row[0], sizeof(records[count].zipcode) - 1);
        if (row[1]) strncpy(records[count].sido, row[1], sizeof(records[count].sido) - 1);
        if (row[2]) strncpy(records[count].gugun, row[2], sizeof(records[count].gugun) - 1);
        if (row[3]) strncpy(records[count].dong, row[3], sizeof(records[count].dong) - 1);
        if (row[4]) strncpy(records[count].bunji, row[4], sizeof(records[count].bunji) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
