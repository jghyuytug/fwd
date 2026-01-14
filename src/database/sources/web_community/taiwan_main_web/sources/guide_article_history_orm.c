#include "guide_article_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideArticleHistory_Add(DBConnectionManager* manager, const GuideArticleHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_article_history (bbs_cd, bbs_no, occ_time, contents) "
        "VALUES (%d, %d, %d, '%s')",
        record->bbs_cd, record->bbs_no, record->occ_time, record->contents);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideArticleHistory_Get(DBConnectionManager* manager, int no, GuideArticleHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_cd, bbs_no, occ_time, contents FROM guide_article_history WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideArticleHistory));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->bbs_cd = row[1] ? atoi(row[1]) : 0;
    record->bbs_no = row[2] ? atoi(row[2]) : 0;
    record->occ_time = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->contents, row[4], sizeof(record->contents) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuideArticleHistory_GetAll(DBConnectionManager* manager, GuideArticleHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_cd, bbs_no, occ_time, contents FROM guide_article_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideArticleHistory));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_cd = row[1] ? atoi(row[1]) : 0;
        records[count].bbs_no = row[2] ? atoi(row[2]) : 0;
        records[count].occ_time = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].contents, row[4], sizeof(records[count].contents) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
