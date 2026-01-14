#include "penpic_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Penpic_Add(DBConnectionManager* manager, const Penpic* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO penpic (penpic_type, notice, category, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, comment, view, recom, point, adorn, adorn_color1, adorn_color2) "
        "VALUES (%d, %d, %d, '%s', %d, '%s', %d, %d, %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d)",
        record->penpic_type, record->notice, record->category, record->m_nickname, record->m_id, record->m_user_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->title, record->create_day, record->comment, record->view, record->recom, record->point, record->adorn, record->adorn_color1, record->adorn_color2);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Penpic_Get(DBConnectionManager* manager, int no, Penpic* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, penpic_type, notice, category, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, comment, view, recom, point, adorn, adorn_color1, adorn_color2 FROM penpic WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Penpic));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->penpic_type = row[1] ? atoi(row[1]) : 0;
    record->notice = row[2] ? row[2][0] : '\0';
    record->category = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->m_nickname, row[4], sizeof(record->m_nickname) - 1);
    record->m_id = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->m_user_id, row[6], sizeof(record->m_user_id) - 1);
    record->m_sex = row[7] ? row[7][0] : '\0';
    record->server_id = row[8] ? atoi(row[8]) : 0;
    record->charac_no = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->charac_name, row[10], sizeof(record->charac_name) - 1);
    record->job = row[11] ? atoi(row[11]) : 0;
    if (row[12]) strncpy(record->title, row[12], sizeof(record->title) - 1);
    record->create_day = row[13] ? atoi(row[13]) : 0;
    record->comment = row[14] ? atoi(row[14]) : 0;
    record->view = row[15] ? atoi(row[15]) : 0;
    record->recom = row[16] ? atoi(row[16]) : 0;
    record->point = row[17] ? atoi(row[17]) : 0;
    record->adorn = row[18] ? atoi(row[18]) : 0;
    record->adorn_color1 = row[19] ? atoi(row[19]) : 0;
    record->adorn_color2 = row[20] ? atoi(row[20]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Penpic_GetAll(DBConnectionManager* manager, Penpic* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, penpic_type, notice, category, m_nickname, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, comment, view, recom, point, adorn, adorn_color1, adorn_color2 FROM penpic");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Penpic));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].penpic_type = row[1] ? atoi(row[1]) : 0;
        records[count].notice = row[2] ? row[2][0] : '\0';
        records[count].category = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].m_nickname, row[4], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].m_user_id, row[6], sizeof(records[count].m_user_id) - 1);
        records[count].m_sex = row[7] ? row[7][0] : '\0';
        records[count].server_id = row[8] ? atoi(row[8]) : 0;
        records[count].charac_no = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].charac_name, row[10], sizeof(records[count].charac_name) - 1);
        records[count].job = row[11] ? atoi(row[11]) : 0;
        if (row[12]) strncpy(records[count].title, row[12], sizeof(records[count].title) - 1);
        records[count].create_day = row[13] ? atoi(row[13]) : 0;
        records[count].comment = row[14] ? atoi(row[14]) : 0;
        records[count].view = row[15] ? atoi(row[15]) : 0;
        records[count].recom = row[16] ? atoi(row[16]) : 0;
        records[count].point = row[17] ? atoi(row[17]) : 0;
        records[count].adorn = row[18] ? atoi(row[18]) : 0;
        records[count].adorn_color1 = row[19] ? atoi(row[19]) : 0;
        records[count].adorn_color2 = row[20] ? atoi(row[20]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
