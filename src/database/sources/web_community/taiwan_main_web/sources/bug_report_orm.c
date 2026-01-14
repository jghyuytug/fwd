#include "bug_report_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BugReport_Add(DBConnectionManager* manager, const BugReport* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO bug_report (m_id, server_id, charac_no, charac_name, occ_date1, occ_date2, enroll_date, title, comment, attachment1, attachment2, attachment3, bug_class) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->m_id, record->server_id, record->charac_no, record->charac_name, record->occ_date1, record->occ_date2, record->enroll_date, record->title, record->comment, record->attachment1, record->attachment2, record->attachment3, record->bug_class);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BugReport_Get(DBConnectionManager* manager, int no, BugReport* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, server_id, charac_no, charac_name, occ_date1, occ_date2, enroll_date, title, comment, attachment1, attachment2, attachment3, bug_class FROM bug_report WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(BugReport));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->occ_date1, row[5], sizeof(record->occ_date1) - 1);
    if (row[6]) strncpy(record->occ_date2, row[6], sizeof(record->occ_date2) - 1);
    if (row[7]) strncpy(record->enroll_date, row[7], sizeof(record->enroll_date) - 1);
    if (row[8]) strncpy(record->title, row[8], sizeof(record->title) - 1);
    if (row[9]) strncpy(record->comment, row[9], sizeof(record->comment) - 1);
    if (row[10]) strncpy(record->attachment1, row[10], sizeof(record->attachment1) - 1);
    if (row[11]) strncpy(record->attachment2, row[11], sizeof(record->attachment2) - 1);
    if (row[12]) strncpy(record->attachment3, row[12], sizeof(record->attachment3) - 1);
    if (row[13]) strncpy(record->bug_class, row[13], sizeof(record->bug_class) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int BugReport_GetAll(DBConnectionManager* manager, BugReport* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, server_id, charac_no, charac_name, occ_date1, occ_date2, enroll_date, title, comment, attachment1, attachment2, attachment3, bug_class FROM bug_report");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BugReport));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].occ_date1, row[5], sizeof(records[count].occ_date1) - 1);
        if (row[6]) strncpy(records[count].occ_date2, row[6], sizeof(records[count].occ_date2) - 1);
        if (row[7]) strncpy(records[count].enroll_date, row[7], sizeof(records[count].enroll_date) - 1);
        if (row[8]) strncpy(records[count].title, row[8], sizeof(records[count].title) - 1);
        if (row[9]) strncpy(records[count].comment, row[9], sizeof(records[count].comment) - 1);
        if (row[10]) strncpy(records[count].attachment1, row[10], sizeof(records[count].attachment1) - 1);
        if (row[11]) strncpy(records[count].attachment2, row[11], sizeof(records[count].attachment2) - 1);
        if (row[12]) strncpy(records[count].attachment3, row[12], sizeof(records[count].attachment3) - 1);
        if (row[13]) strncpy(records[count].bug_class, row[13], sizeof(records[count].bug_class) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
