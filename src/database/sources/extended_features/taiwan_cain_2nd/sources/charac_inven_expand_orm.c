#include "charac_inven_expand_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacInvenExpand_Add(DBConnectionManager* manager, const CharacInvenExpand* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_inven_expand (charac_no, cargo_capacity, current_equipslot) "
        "VALUES (%d, %d, '%s')",
        record->charac_no, record->cargo_capacity, record->current_equipslot);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacInvenExpand_Get(DBConnectionManager* manager, int charac_no, CharacInvenExpand* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, cargo_capacity, current_equipslot FROM charac_inven_expand WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacInvenExpand));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->cargo_capacity = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->current_equipslot, row[2], sizeof(record->current_equipslot) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CharacInvenExpand_GetAll(DBConnectionManager* manager, CharacInvenExpand* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, cargo_capacity, current_equipslot FROM charac_inven_expand");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacInvenExpand));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].cargo_capacity = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].current_equipslot, row[2], sizeof(records[count].current_equipslot) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
