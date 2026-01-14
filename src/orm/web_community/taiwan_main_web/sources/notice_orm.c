#include "notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Notice_Add(DBConnectionManager* manager, const Notice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO notice (bbs_name, category, m_nickname, m_id, m_user_id, m_sex, title, create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, depth, sequence, content, content_type, ip, ring, sms) "
        "VALUES ('%s', %d, '%s', %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %f, '%s', %d, '%s', %d, %d)",
        record->bbs_name, record->category, record->m_nickname, record->m_id, record->m_user_id, record->m_sex, record->title, record->create_day, record->comment, record->view, record->recom, record->adorn, record->adorn_color1, record->adorn_color2, record->depth, record->sequence, record->content, record->content_type, record->ip, record->ring, record->sms);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Notice_GetAll(DBConnectionManager* manager, Notice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[22];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_name, no, category, m_nickname, m_id, m_user_id, m_sex, title, create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, depth, sequence, content, content_type, ip, ring, sms FROM notice");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Notice));
        if (row[0]) strncpy(records[count].bbs_name, row[0], sizeof(records[count].bbs_name) - 1);
        records[count].no = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].m_nickname, row[3], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].m_user_id, row[5], sizeof(records[count].m_user_id) - 1);
        records[count].m_sex = row[6] ? row[6][0] : '\0';
        if (row[7]) strncpy(records[count].title, row[7], sizeof(records[count].title) - 1);
        records[count].create_day = row[8] ? atoi(row[8]) : 0;
        records[count].comment = row[9] ? atoi(row[9]) : 0;
        records[count].view = row[10] ? atoi(row[10]) : 0;
        records[count].recom = row[11] ? atoi(row[11]) : 0;
        records[count].adorn = row[12] ? atoi(row[12]) : 0;
        records[count].adorn_color1 = row[13] ? atoi(row[13]) : 0;
        records[count].adorn_color2 = row[14] ? atoi(row[14]) : 0;
        records[count].depth = row[15] ? atoi(row[15]) : 0;
        records[count].sequence = row[16] ? atof(row[16]) : 0;
        if (row[17]) strncpy(records[count].content, row[17], sizeof(records[count].content) - 1);
        records[count].content_type = row[18] ? row[18][0] : '\0';
        if (row[19]) strncpy(records[count].ip, row[19], sizeof(records[count].ip) - 1);
        records[count].ring = row[20] ? atoi(row[20]) : 0;
        records[count].sms = row[21] ? row[21][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
