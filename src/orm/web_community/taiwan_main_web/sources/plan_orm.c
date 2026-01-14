#include "plan_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Plan_Add(DBConnectionManager* manager, const Plan* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO plan (notice, category, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, sub_job, lev, title, create_day, comment, view, recom, file) "
        "VALUES (%d, %d, '%s', %d, '%s', %d, %d, %d, '%s', %d, %d, %d, '%s', %d, %d, %d, %d, %d)",
        record->notice, record->category, record->m_nickname, record->m_id, record->m_user_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->sub_job, record->lev, record->title, record->create_day, record->comment, record->view, record->recom, record->file);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Plan_Get(DBConnectionManager* manager, int no, Plan* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, sub_job, lev, title, create_day, comment, view, recom, file FROM plan WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Plan));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->notice = row[1] ? row[1][0] : '\0';
    record->category = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->m_nickname, row[3], sizeof(record->m_nickname) - 1);
    record->m_id = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->m_user_id, row[5], sizeof(record->m_user_id) - 1);
    record->m_sex = row[6] ? row[6][0] : '\0';
    record->server_id = row[7] ? atoi(row[7]) : 0;
    record->charac_no = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->charac_name, row[9], sizeof(record->charac_name) - 1);
    record->job = row[10] ? atoi(row[10]) : 0;
    record->sub_job = row[11] ? atoi(row[11]) : 0;
    record->lev = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->title, row[13], sizeof(record->title) - 1);
    record->create_day = row[14] ? atoi(row[14]) : 0;
    record->comment = row[15] ? atoi(row[15]) : 0;
    record->view = row[16] ? atoi(row[16]) : 0;
    record->recom = row[17] ? atoi(row[17]) : 0;
    record->file = row[18] ? atoi(row[18]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Plan_GetAll(DBConnectionManager* manager, Plan* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, sub_job, lev, title, create_day, comment, view, recom, file FROM plan");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Plan));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].notice = row[1] ? row[1][0] : '\0';
        records[count].category = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].m_nickname, row[3], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].m_user_id, row[5], sizeof(records[count].m_user_id) - 1);
        records[count].m_sex = row[6] ? row[6][0] : '\0';
        records[count].server_id = row[7] ? atoi(row[7]) : 0;
        records[count].charac_no = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].charac_name, row[9], sizeof(records[count].charac_name) - 1);
        records[count].job = row[10] ? atoi(row[10]) : 0;
        records[count].sub_job = row[11] ? atoi(row[11]) : 0;
        records[count].lev = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].title, row[13], sizeof(records[count].title) - 1);
        records[count].create_day = row[14] ? atoi(row[14]) : 0;
        records[count].comment = row[15] ? atoi(row[15]) : 0;
        records[count].view = row[16] ? atoi(row[16]) : 0;
        records[count].recom = row[17] ? atoi(row[17]) : 0;
        records[count].file = row[18] ? atoi(row[18]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
