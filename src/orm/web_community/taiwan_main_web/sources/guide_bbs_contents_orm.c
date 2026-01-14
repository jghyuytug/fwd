#include "guide_bbs_contents_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideBbsContents_Add(DBConnectionManager* manager, const GuideBbsContents* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_bbs_contents (bbs_no, contents) "
        "VALUES (%d, '%s')",
        record->bbs_no, record->contents);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbsContents_Get(DBConnectionManager* manager, int no, GuideBbsContents* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, contents FROM guide_bbs_contents WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideBbsContents));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->bbs_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->contents, row[2], sizeof(record->contents) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbsContents_GetAll(DBConnectionManager* manager, GuideBbsContents* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, contents FROM guide_bbs_contents");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideBbsContents));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].contents, row[2], sizeof(records[count].contents) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
