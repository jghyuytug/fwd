#include "wiki_mng_encyclopedia_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WikiMngEncyclopedia_Add(DBConnectionManager* manager, const WikiMngEncyclopedia* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO wiki_mng_encyclopedia (category, subject, content, reg_date, hit, today_info, today_char, pupular_flag, img_url, word) "
        "VALUES (%d, '%s', '%s', %d, %d, %d, %d, %d, '%s', '%s')",
        record->category, record->subject, record->content, record->reg_date, record->hit, record->today_info, record->today_char, record->pupular_flag, record->img_url, record->word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WikiMngEncyclopedia_Get(DBConnectionManager* manager, int id, WikiMngEncyclopedia* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, category, subject, content, reg_date, hit, today_info, today_char, pupular_flag, img_url, word FROM wiki_mng_encyclopedia WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(WikiMngEncyclopedia));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->category = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->subject, row[2], sizeof(record->subject) - 1);
    if (row[3]) strncpy(record->content, row[3], sizeof(record->content) - 1);
    record->reg_date = row[4] ? atoi(row[4]) : 0;
    record->hit = row[5] ? atoi(row[5]) : 0;
    record->today_info = row[6] ? atoi(row[6]) : 0;
    record->today_char = row[7] ? atoi(row[7]) : 0;
    record->pupular_flag = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->img_url, row[9], sizeof(record->img_url) - 1);
    if (row[10]) strncpy(record->word, row[10], sizeof(record->word) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int WikiMngEncyclopedia_GetAll(DBConnectionManager* manager, WikiMngEncyclopedia* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, category, subject, content, reg_date, hit, today_info, today_char, pupular_flag, img_url, word FROM wiki_mng_encyclopedia");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WikiMngEncyclopedia));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].category = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].subject, row[2], sizeof(records[count].subject) - 1);
        if (row[3]) strncpy(records[count].content, row[3], sizeof(records[count].content) - 1);
        records[count].reg_date = row[4] ? atoi(row[4]) : 0;
        records[count].hit = row[5] ? atoi(row[5]) : 0;
        records[count].today_info = row[6] ? atoi(row[6]) : 0;
        records[count].today_char = row[7] ? atoi(row[7]) : 0;
        records[count].pupular_flag = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].img_url, row[9], sizeof(records[count].img_url) - 1);
        if (row[10]) strncpy(records[count].word, row[10], sizeof(records[count].word) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
