#include "dictionary_stats_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DictionaryStats_Add(DBConnectionManager* manager, const DictionaryStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dictionary_stats (date_enroll, dic_name, every_view_cnt, detail_view_cnt, comment_cnt) "
        "VALUES ('%s', '%s', %d, %d, %d)",
        record->date_enroll, record->dic_name, record->every_view_cnt, record->detail_view_cnt, record->comment_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DictionaryStats_Get(DBConnectionManager* manager, const char* date_enroll, const char* dic_name, DictionaryStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT date_enroll, dic_name, every_view_cnt, detail_view_cnt, comment_cnt FROM dictionary_stats WHERE date_enroll = '%s' AND dic_name = '%s'",
        date_enroll, dic_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DictionaryStats));
    if (row[0]) strncpy(record->date_enroll, row[0], sizeof(record->date_enroll) - 1);
    if (row[1]) strncpy(record->dic_name, row[1], sizeof(record->dic_name) - 1);
    record->every_view_cnt = row[2] ? atoi(row[2]) : 0;
    record->detail_view_cnt = row[3] ? atoi(row[3]) : 0;
    record->comment_cnt = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DictionaryStats_GetAll(DBConnectionManager* manager, DictionaryStats* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT date_enroll, dic_name, every_view_cnt, detail_view_cnt, comment_cnt FROM dictionary_stats");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DictionaryStats));
        if (row[0]) strncpy(records[count].date_enroll, row[0], sizeof(records[count].date_enroll) - 1);
        if (row[1]) strncpy(records[count].dic_name, row[1], sizeof(records[count].dic_name) - 1);
        records[count].every_view_cnt = row[2] ? atoi(row[2]) : 0;
        records[count].detail_view_cnt = row[3] ? atoi(row[3]) : 0;
        records[count].comment_cnt = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
