#include "knowledge_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Knowledge_Add(DBConnectionManager* manager, const Knowledge* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowledge (kn_id, question_flag, m_id, server_id, charac_no, charac_name, job, title, know_flag, create_day, comment, view, value, file) "
        "VALUES (%d, %d, %d, %d, %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d)",
        record->kn_id, record->question_flag, record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->title, record->know_flag, record->create_day, record->comment, record->view, record->value, record->file);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Knowledge_Get(DBConnectionManager* manager, int no, Knowledge* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, kn_id, question_flag, m_id, server_id, charac_no, charac_name, job, title, know_flag, create_day, comment, view, value, file FROM knowledge WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Knowledge));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->kn_id = row[1] ? atoll(row[1]) : 0;
    record->question_flag = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    record->server_id = row[4] ? atoi(row[4]) : 0;
    record->charac_no = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->charac_name, row[6], sizeof(record->charac_name) - 1);
    record->job = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->title, row[8], sizeof(record->title) - 1);
    record->know_flag = row[9] ? atoi(row[9]) : 0;
    record->create_day = row[10] ? atoi(row[10]) : 0;
    record->comment = row[11] ? atoi(row[11]) : 0;
    record->view = row[12] ? atoi(row[12]) : 0;
    record->value = row[13] ? atoi(row[13]) : 0;
    record->file = row[14] ? atoi(row[14]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Knowledge_GetAll(DBConnectionManager* manager, Knowledge* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, kn_id, question_flag, m_id, server_id, charac_no, charac_name, job, title, know_flag, create_day, comment, view, value, file FROM knowledge");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Knowledge));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].kn_id = row[1] ? atoll(row[1]) : 0;
        records[count].question_flag = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        records[count].server_id = row[4] ? atoi(row[4]) : 0;
        records[count].charac_no = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].charac_name, row[6], sizeof(records[count].charac_name) - 1);
        records[count].job = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].title, row[8], sizeof(records[count].title) - 1);
        records[count].know_flag = row[9] ? atoi(row[9]) : 0;
        records[count].create_day = row[10] ? atoi(row[10]) : 0;
        records[count].comment = row[11] ? atoi(row[11]) : 0;
        records[count].view = row[12] ? atoi(row[12]) : 0;
        records[count].value = row[13] ? atoi(row[13]) : 0;
        records[count].file = row[14] ? atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
