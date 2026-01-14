#include "tcg_free_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TcgFree_Add(DBConnectionManager* manager, const TcgFree* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tcg_free (notice, category, m_id, m_sex, charac_no, charac_name, job, title, create_day, comment, view, file) "
        "VALUES (%d, %d, %d, %d, %d, '%s', %d, '%s', %d, %d, %d, %d)",
        record->notice, record->category, record->m_id, record->m_sex, record->charac_no, record->charac_name, record->job, record->title, record->create_day, record->comment, record->view, record->file);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TcgFree_Get(DBConnectionManager* manager, int no, TcgFree* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, m_id, m_sex, charac_no, charac_name, job, title, create_day, comment, view, file FROM tcg_free WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TcgFree));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->notice = row[1] ? row[1][0] : '\0';
    record->category = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    record->m_sex = row[4] ? row[4][0] : '\0';
    record->charac_no = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->charac_name, row[6], sizeof(record->charac_name) - 1);
    record->job = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->title, row[8], sizeof(record->title) - 1);
    record->create_day = row[9] ? atoi(row[9]) : 0;
    record->comment = row[10] ? atoi(row[10]) : 0;
    record->view = row[11] ? atoi(row[11]) : 0;
    record->file = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int TcgFree_GetAll(DBConnectionManager* manager, TcgFree* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, m_id, m_sex, charac_no, charac_name, job, title, create_day, comment, view, file FROM tcg_free");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TcgFree));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].notice = row[1] ? row[1][0] : '\0';
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        records[count].m_sex = row[4] ? row[4][0] : '\0';
        records[count].charac_no = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].charac_name, row[6], sizeof(records[count].charac_name) - 1);
        records[count].job = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].title, row[8], sizeof(records[count].title) - 1);
        records[count].create_day = row[9] ? atoi(row[9]) : 0;
        records[count].comment = row[10] ? atoi(row[10]) : 0;
        records[count].view = row[11] ? atoi(row[11]) : 0;
        records[count].file = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
