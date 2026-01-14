#include "guide_bbs_link_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideBbsLink_Add(DBConnectionManager* manager, const GuideBbsLink* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_bbs_link (bbs_no, link_kind, title, link_url) "
        "VALUES (%d, %d, '%s', '%s')",
        record->bbs_no, record->link_kind, record->title, record->link_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbsLink_Get(DBConnectionManager* manager, int no, GuideBbsLink* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, link_kind, title, link_url FROM guide_bbs_link WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideBbsLink));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->bbs_no = row[1] ? atoi(row[1]) : 0;
    record->link_kind = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->title, row[3], sizeof(record->title) - 1);
    if (row[4]) strncpy(record->link_url, row[4], sizeof(record->link_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuideBbsLink_GetAll(DBConnectionManager* manager, GuideBbsLink* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_no, link_kind, title, link_url FROM guide_bbs_link");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideBbsLink));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_no = row[1] ? atoi(row[1]) : 0;
        records[count].link_kind = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].title, row[3], sizeof(records[count].title) - 1);
        if (row[4]) strncpy(records[count].link_url, row[4], sizeof(records[count].link_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
