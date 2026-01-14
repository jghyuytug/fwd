#include "penpic2_valuation_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Penpic2Valuation_Add(DBConnectionManager* manager, const Penpic2Valuation* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO penpic2_valuation (no, penpic_desc, penpic_examine) "
        "VALUES (%d, '%s', '%s')",
        record->no, record->penpic_desc, record->penpic_examine);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Penpic2Valuation_Get(DBConnectionManager* manager, int no, Penpic2Valuation* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, penpic_desc, penpic_examine FROM penpic2_valuation WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Penpic2Valuation));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->penpic_desc, row[1], sizeof(record->penpic_desc) - 1);
    if (row[2]) strncpy(record->penpic_examine, row[2], sizeof(record->penpic_examine) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Penpic2Valuation_GetAll(DBConnectionManager* manager, Penpic2Valuation* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, penpic_desc, penpic_examine FROM penpic2_valuation");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Penpic2Valuation));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].penpic_desc, row[1], sizeof(records[count].penpic_desc) - 1);
        if (row[2]) strncpy(records[count].penpic_examine, row[2], sizeof(records[count].penpic_examine) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
