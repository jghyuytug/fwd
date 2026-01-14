#include "client_down_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ClientDown_Add(DBConnectionManager* manager, const ClientDown* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO client_down (occ_date, occ_count) "
        "VALUES ('%s', %d)",
        record->occ_date, record->occ_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ClientDown_Get(DBConnectionManager* manager, const char* occ_date, ClientDown* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, occ_count FROM client_down WHERE occ_date = '%s'",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ClientDown));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->occ_count = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ClientDown_GetAll(DBConnectionManager* manager, ClientDown* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, occ_count FROM client_down");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ClientDown));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].occ_count = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
