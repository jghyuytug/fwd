#include "wiki_mng_guide_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WikiMngGuide_Add(DBConnectionManager* manager, const WikiMngGuide* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO wiki_mng_guide (url_go, url_to, reg_date, subject, hits, content, flag, p_type) "
        "VALUES ('%s', '%s', %d, '%s', %d, '%s', %d, %d)",
        record->url_go, record->url_to, record->reg_date, record->subject, record->hits, record->content, record->flag, record->p_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WikiMngGuide_Get(DBConnectionManager* manager, int id, WikiMngGuide* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, url_go, url_to, reg_date, subject, hits, content, flag, p_type FROM wiki_mng_guide WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(WikiMngGuide));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->url_go, row[1], sizeof(record->url_go) - 1);
    if (row[2]) strncpy(record->url_to, row[2], sizeof(record->url_to) - 1);
    record->reg_date = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->subject, row[4], sizeof(record->subject) - 1);
    record->hits = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->content, row[6], sizeof(record->content) - 1);
    record->flag = row[7] ? atoi(row[7]) : 0;
    record->p_type = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int WikiMngGuide_GetAll(DBConnectionManager* manager, WikiMngGuide* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, url_go, url_to, reg_date, subject, hits, content, flag, p_type FROM wiki_mng_guide");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WikiMngGuide));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].url_go, row[1], sizeof(records[count].url_go) - 1);
        if (row[2]) strncpy(records[count].url_to, row[2], sizeof(records[count].url_to) - 1);
        records[count].reg_date = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].subject, row[4], sizeof(records[count].subject) - 1);
        records[count].hits = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].content, row[6], sizeof(records[count].content) - 1);
        records[count].flag = row[7] ? atoi(row[7]) : 0;
        records[count].p_type = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
