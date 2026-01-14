#include "wiki_agreement_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WikiAgreement_Add(DBConnectionManager* manager, const WikiAgreement* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO wiki_agreement (url_go, url_to, reg_date, subject, content) "
        "VALUES ('%s', '%s', %d, '%s', '%s')",
        record->url_go, record->url_to, record->reg_date, record->subject, record->content);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WikiAgreement_Get(DBConnectionManager* manager, int id, WikiAgreement* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, url_go, url_to, reg_date, subject, content FROM wiki_agreement WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(WikiAgreement));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->url_go, row[1], sizeof(record->url_go) - 1);
    if (row[2]) strncpy(record->url_to, row[2], sizeof(record->url_to) - 1);
    record->reg_date = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->subject, row[4], sizeof(record->subject) - 1);
    if (row[5]) strncpy(record->content, row[5], sizeof(record->content) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int WikiAgreement_GetAll(DBConnectionManager* manager, WikiAgreement* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, url_go, url_to, reg_date, subject, content FROM wiki_agreement");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WikiAgreement));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].url_go, row[1], sizeof(records[count].url_go) - 1);
        if (row[2]) strncpy(records[count].url_to, row[2], sizeof(records[count].url_to) - 1);
        records[count].reg_date = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].subject, row[4], sizeof(records[count].subject) - 1);
        if (row[5]) strncpy(records[count].content, row[5], sizeof(records[count].content) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
