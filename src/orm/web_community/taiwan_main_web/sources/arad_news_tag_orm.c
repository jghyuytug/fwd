#include "arad_news_tag_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AradNewsTag_Add(DBConnectionManager* manager, const AradNewsTag* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO arad_news_tag (occ_date, tag, tag_cnt, rank) "
        "VALUES ('%s', '%s', %d, %d)",
        record->occ_date, record->tag, record->tag_cnt, record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsTag_Get(DBConnectionManager* manager, const char* occ_date, const char* tag, int rank, AradNewsTag* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, tag, tag_cnt, rank FROM arad_news_tag WHERE occ_date = '%s' AND tag = '%s' AND rank = %d",
        occ_date, tag, rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AradNewsTag));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    if (row[1]) strncpy(record->tag, row[1], sizeof(record->tag) - 1);
    record->tag_cnt = row[2] ? atoi(row[2]) : 0;
    record->rank = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsTag_GetAll(DBConnectionManager* manager, AradNewsTag* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, tag, tag_cnt, rank FROM arad_news_tag");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AradNewsTag));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        if (row[1]) strncpy(records[count].tag, row[1], sizeof(records[count].tag) - 1);
        records[count].tag_cnt = row[2] ? atoi(row[2]) : 0;
        records[count].rank = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
