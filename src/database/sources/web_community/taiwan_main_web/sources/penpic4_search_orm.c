#include "penpic4_search_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Penpic4Search_Add(DBConnectionManager* manager, const Penpic4Search* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO penpic4_search (no, penpic_type, category, class, word, degree) "
        "VALUES (%d, %d, %d, %d, '%s', %d)",
        record->no, record->penpic_type, record->category, record->class, record->word, record->degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Penpic4Search_GetAll(DBConnectionManager* manager, Penpic4Search* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, penpic_type, category, class, word, degree FROM penpic4_search");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Penpic4Search));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].penpic_type = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].class = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(records[count].word, row[4], sizeof(records[count].word) - 1);
        records[count].degree = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
