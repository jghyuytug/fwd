#include "dnf_story_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfStory_Add(DBConnectionManager* manager, const DnfStory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_story (story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', %d, %d, '%s', %d, %d, '%s')",
        record->story_type, record->notice_flag, record->m_id, record->reg_id, record->title, record->url, record->img_name, record->opt, record->open_flag, record->reg_date, record->hits, record->reserve_time, record->content);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_Get(DBConnectionManager* manager, int no, DnfStory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content FROM dnf_story WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfStory));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->story_type = row[1] ? atoi(row[1]) : 0;
    record->notice_flag = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->reg_id, row[4], sizeof(record->reg_id) - 1);
    if (row[5]) strncpy(record->title, row[5], sizeof(record->title) - 1);
    if (row[6]) strncpy(record->url, row[6], sizeof(record->url) - 1);
    if (row[7]) strncpy(record->img_name, row[7], sizeof(record->img_name) - 1);
    record->opt = row[8] ? atoi(row[8]) : 0;
    record->open_flag = row[9] ? row[9][0] : '\0';
    if (row[10]) strncpy(record->reg_date, row[10], sizeof(record->reg_date) - 1);
    record->hits = row[11] ? atoi(row[11]) : 0;
    record->reserve_time = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->content, row[13], sizeof(record->content) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_GetAll(DBConnectionManager* manager, DnfStory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content FROM dnf_story");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfStory));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].story_type = row[1] ? atoi(row[1]) : 0;
        records[count].notice_flag = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].reg_id, row[4], sizeof(records[count].reg_id) - 1);
        if (row[5]) strncpy(records[count].title, row[5], sizeof(records[count].title) - 1);
        if (row[6]) strncpy(records[count].url, row[6], sizeof(records[count].url) - 1);
        if (row[7]) strncpy(records[count].img_name, row[7], sizeof(records[count].img_name) - 1);
        records[count].opt = row[8] ? atoi(row[8]) : 0;
        records[count].open_flag = row[9] ? row[9][0] : '\0';
        if (row[10]) strncpy(records[count].reg_date, row[10], sizeof(records[count].reg_date) - 1);
        records[count].hits = row[11] ? atoi(row[11]) : 0;
        records[count].reserve_time = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].content, row[13], sizeof(records[count].content) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
