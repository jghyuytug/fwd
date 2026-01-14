#include "rdar0_content_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Rdar0Content_Add(DBConnectionManager* manager, const Rdar0Content* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO rdar0_content (no, content, content_type, ip, ring, sms) "
        "VALUES (%d, '%s', %d, '%s', %d, %d)",
        record->no, record->content, record->content_type, record->ip, record->ring, record->sms);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Rdar0Content_Get(DBConnectionManager* manager, int no, Rdar0Content* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content, content_type, ip, ring, sms FROM rdar0_content WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Rdar0Content));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->content, row[1], sizeof(record->content) - 1);
    record->content_type = row[2] ? row[2][0] : '\0';
    if (row[3]) strncpy(record->ip, row[3], sizeof(record->ip) - 1);
    record->ring = row[4] ? atoi(row[4]) : 0;
    record->sms = row[5] ? row[5][0] : '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int Rdar0Content_GetAll(DBConnectionManager* manager, Rdar0Content* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content, content_type, ip, ring, sms FROM rdar0_content");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Rdar0Content));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].content, row[1], sizeof(records[count].content) - 1);
        records[count].content_type = row[2] ? row[2][0] : '\0';
        if (row[3]) strncpy(records[count].ip, row[3], sizeof(records[count].ip) - 1);
        records[count].ring = row[4] ? atoi(row[4]) : 0;
        records[count].sms = row[5] ? row[5][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
