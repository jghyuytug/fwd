#include "dnf_faq_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfFaq_Add(DBConnectionManager* manager, const DnfFaq* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_faq (cat1, cat2, regdate, hit, subject) "
        "VALUES (%d, %d, %d, %d, '%s')",
        record->cat1, record->cat2, record->regdate, record->hit, record->subject);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfFaq_Get(DBConnectionManager* manager, int no, DnfFaq* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, cat1, cat2, regdate, hit, subject FROM dnf_faq WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfFaq));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->cat1 = row[1] ? atoi(row[1]) : 0;
    record->cat2 = row[2] ? atoi(row[2]) : 0;
    record->regdate = row[3] ? atoi(row[3]) : 0;
    record->hit = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->subject, row[5], sizeof(record->subject) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfFaq_GetAll(DBConnectionManager* manager, DnfFaq* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, cat1, cat2, regdate, hit, subject FROM dnf_faq");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfFaq));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].cat1 = row[1] ? atoi(row[1]) : 0;
        records[count].cat2 = row[2] ? atoi(row[2]) : 0;
        records[count].regdate = row[3] ? atoi(row[3]) : 0;
        records[count].hit = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].subject, row[5], sizeof(records[count].subject) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
