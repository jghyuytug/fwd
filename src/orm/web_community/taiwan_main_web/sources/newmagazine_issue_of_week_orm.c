#include "newmagazine_issue_of_week_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int NewmagazineIssueOfWeek_Add(DBConnectionManager* manager, const NewmagazineIssueOfWeek* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO newmagazine_issue_of_week (occ_date, title, contents, dnf_main) "
        "VALUES ('%s', '%s', '%s', '%s')",
        record->occ_date, record->title, record->contents, record->dnf_main);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineIssueOfWeek_Get(DBConnectionManager* manager, int no, NewmagazineIssueOfWeek* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, occ_date, title, contents, dnf_main FROM newmagazine_issue_of_week WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(NewmagazineIssueOfWeek));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    if (row[2]) strncpy(record->title, row[2], sizeof(record->title) - 1);
    if (row[3]) strncpy(record->contents, row[3], sizeof(record->contents) - 1);
    if (row[4]) strncpy(record->dnf_main, row[4], sizeof(record->dnf_main) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineIssueOfWeek_GetAll(DBConnectionManager* manager, NewmagazineIssueOfWeek* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, occ_date, title, contents, dnf_main FROM newmagazine_issue_of_week");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(NewmagazineIssueOfWeek));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        if (row[2]) strncpy(records[count].title, row[2], sizeof(records[count].title) - 1);
        if (row[3]) strncpy(records[count].contents, row[3], sizeof(records[count].contents) - 1);
        if (row[4]) strncpy(records[count].dnf_main, row[4], sizeof(records[count].dnf_main) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
