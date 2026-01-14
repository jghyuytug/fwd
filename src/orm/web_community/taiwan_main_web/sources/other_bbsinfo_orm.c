#include "other_bbsinfo_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OtherBbsinfo_Add(DBConnectionManager* manager, const OtherBbsinfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO other_bbsinfo (kn_id, url_go, url_to, hits, reg_date, subject, ref_url, content) "
        "VALUES (%d, '%s', '%s', %d, %d, '%s', '%s', '%s')",
        record->kn_id, record->url_go, record->url_to, record->hits, record->reg_date, record->subject, record->ref_url, record->content);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OtherBbsinfo_Get(DBConnectionManager* manager, const char* url_go, const char* url_to, OtherBbsinfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, url_go, url_to, hits, reg_date, subject, ref_url, content FROM other_bbsinfo WHERE url_go = '%s' AND url_to = '%s'",
        url_go, url_to);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OtherBbsinfo));
    record->kn_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->url_go, row[1], sizeof(record->url_go) - 1);
    if (row[2]) strncpy(record->url_to, row[2], sizeof(record->url_to) - 1);
    record->hits = row[3] ? atoi(row[3]) : 0;
    record->reg_date = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->subject, row[5], sizeof(record->subject) - 1);
    if (row[6]) strncpy(record->ref_url, row[6], sizeof(record->ref_url) - 1);
    if (row[7]) strncpy(record->content, row[7], sizeof(record->content) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int OtherBbsinfo_GetAll(DBConnectionManager* manager, OtherBbsinfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, url_go, url_to, hits, reg_date, subject, ref_url, content FROM other_bbsinfo");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OtherBbsinfo));
        records[count].kn_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].url_go, row[1], sizeof(records[count].url_go) - 1);
        if (row[2]) strncpy(records[count].url_to, row[2], sizeof(records[count].url_to) - 1);
        records[count].hits = row[3] ? atoi(row[3]) : 0;
        records[count].reg_date = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].subject, row[5], sizeof(records[count].subject) - 1);
        if (row[6]) strncpy(records[count].ref_url, row[6], sizeof(records[count].ref_url) - 1);
        if (row[7]) strncpy(records[count].content, row[7], sizeof(records[count].content) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
