#include "dnf_faq_content_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfFaqContent_Add(DBConnectionManager* manager, const DnfFaqContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_faq_content (no, contents) "
        "VALUES (%d, '%s')",
        record->no, record->contents);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfFaqContent_Get(DBConnectionManager* manager, int no, DnfFaqContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, contents FROM dnf_faq_content WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfFaqContent));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->contents, row[1], sizeof(record->contents) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfFaqContent_GetAll(DBConnectionManager* manager, DnfFaqContent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, contents FROM dnf_faq_content");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfFaqContent));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].contents, row[1], sizeof(records[count].contents) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
