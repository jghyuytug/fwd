#include "guide_bbs_search_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideBbsSearch_Add(DBConnectionManager* manager, const GuideBbsSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_bbs_search (bbs_no, search_text) "
        "VALUES (%d, '%s')",
        record->bbs_no, record->search_text);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbsSearch_Get(DBConnectionManager* manager, int no, GuideBbsSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, search_text FROM guide_bbs_search WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideBbsSearch));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->bbs_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->search_text, row[2], sizeof(record->search_text) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbsSearch_GetAll(DBConnectionManager* manager, GuideBbsSearch* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, search_text FROM guide_bbs_search");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideBbsSearch));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].search_text, row[2], sizeof(records[count].search_text) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
