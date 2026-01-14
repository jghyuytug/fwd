#include "focus_content_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int FocusContent_Add(DBConnectionManager* manager, const FocusContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO focus_content (focus_no, category, title, popular_word, link, view, best, review) "
        "VALUES (%d, %d, '%s', '%s', '%s', %d, %d, '%s')",
        record->focus_no, record->category, record->title, record->popular_word, record->link, record->view, record->best, record->review);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int FocusContent_Get(DBConnectionManager* manager, int no, FocusContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, focus_no, category, title, popular_word, link, view, best, review FROM focus_content WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(FocusContent));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->focus_no = row[1] ? atoi(row[1]) : 0;
    record->category = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->title, row[3], sizeof(record->title) - 1);
    if (row[4]) strncpy(record->popular_word, row[4], sizeof(record->popular_word) - 1);
    if (row[5]) strncpy(record->link, row[5], sizeof(record->link) - 1);
    record->view = row[6] ? atoi(row[6]) : 0;
    record->best = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->review, row[8], sizeof(record->review) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int FocusContent_GetAll(DBConnectionManager* manager, FocusContent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, focus_no, category, title, popular_word, link, view, best, review FROM focus_content");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(FocusContent));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].focus_no = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].title, row[3], sizeof(records[count].title) - 1);
        if (row[4]) strncpy(records[count].popular_word, row[4], sizeof(records[count].popular_word) - 1);
        if (row[5]) strncpy(records[count].link, row[5], sizeof(records[count].link) - 1);
        records[count].view = row[6] ? atoi(row[6]) : 0;
        records[count].best = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].review, row[8], sizeof(records[count].review) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
