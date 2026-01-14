#include "knowcafe_qa_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeQa_Add(DBConnectionManager* manager, const KnowcafeQa* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_qa (m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, file, reply) "
        "VALUES ('%s', %d, '%s', %d, %d, %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->m_nickname, record->m_id, record->m_user_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->title, record->create_day, record->comment, record->view, record->recom, record->adorn, record->adorn_color1, record->adorn_color2, record->file, record->reply);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeQa_Get(DBConnectionManager* manager, int no, KnowcafeQa* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, file, reply FROM knowcafe_qa WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafeQa));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->m_nickname, row[1], sizeof(record->m_nickname) - 1);
    record->m_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->m_user_id, row[3], sizeof(record->m_user_id) - 1);
    record->m_sex = row[4] ? row[4][0] : '\0';
    record->server_id = row[5] ? atoi(row[5]) : 0;
    record->charac_no = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->charac_name, row[7], sizeof(record->charac_name) - 1);
    record->job = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->title, row[9], sizeof(record->title) - 1);
    record->create_day = row[10] ? atoi(row[10]) : 0;
    record->comment = row[11] ? atoi(row[11]) : 0;
    record->view = row[12] ? atoi(row[12]) : 0;
    record->recom = row[13] ? atoi(row[13]) : 0;
    record->adorn = row[14] ? atoi(row[14]) : 0;
    record->adorn_color1 = row[15] ? atoi(row[15]) : 0;
    record->adorn_color2 = row[16] ? atoi(row[16]) : 0;
    record->file = row[17] ? atoi(row[17]) : 0;
    record->reply = row[18] ? atoi(row[18]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeQa_GetAll(DBConnectionManager* manager, KnowcafeQa* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, file, reply FROM knowcafe_qa");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeQa));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].m_nickname, row[1], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].m_user_id, row[3], sizeof(records[count].m_user_id) - 1);
        records[count].m_sex = row[4] ? row[4][0] : '\0';
        records[count].server_id = row[5] ? atoi(row[5]) : 0;
        records[count].charac_no = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].charac_name, row[7], sizeof(records[count].charac_name) - 1);
        records[count].job = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].title, row[9], sizeof(records[count].title) - 1);
        records[count].create_day = row[10] ? atoi(row[10]) : 0;
        records[count].comment = row[11] ? atoi(row[11]) : 0;
        records[count].view = row[12] ? atoi(row[12]) : 0;
        records[count].recom = row[13] ? atoi(row[13]) : 0;
        records[count].adorn = row[14] ? atoi(row[14]) : 0;
        records[count].adorn_color1 = row[15] ? atoi(row[15]) : 0;
        records[count].adorn_color2 = row[16] ? atoi(row[16]) : 0;
        records[count].file = row[17] ? atoi(row[17]) : 0;
        records[count].reply = row[18] ? atoi(row[18]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
