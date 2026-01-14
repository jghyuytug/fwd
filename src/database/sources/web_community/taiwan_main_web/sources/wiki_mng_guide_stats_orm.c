#include "wiki_mng_guide_stats_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WikiMngGuideStats_Add(DBConnectionManager* manager, const WikiMngGuideStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO wiki_mng_guide_stats (id, date_enroll, url_go, url_to, hits) "
        "VALUES (%d, '%s', '%s', '%s', %d)",
        record->id, record->date_enroll, record->url_go, record->url_to, record->hits);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WikiMngGuideStats_GetAll(DBConnectionManager* manager, WikiMngGuideStats* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, date_enroll, url_go, url_to, hits FROM wiki_mng_guide_stats");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WikiMngGuideStats));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].date_enroll, row[1], sizeof(records[count].date_enroll) - 1);
        if (row[2]) strncpy(records[count].url_go, row[2], sizeof(records[count].url_go) - 1);
        if (row[3]) strncpy(records[count].url_to, row[3], sizeof(records[count].url_to) - 1);
        records[count].hits = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
