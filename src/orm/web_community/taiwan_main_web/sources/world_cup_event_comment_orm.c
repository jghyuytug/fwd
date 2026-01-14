#include "world_cup_event_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WorldCupEventComment_Add(DBConnectionManager* manager, const WorldCupEventComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO world_cup_event_comment (game_type, score1, score2, m_id, server_id, charac_no, charac_name, comment_text, create_day) "
        "VALUES (%d, %d, %d, %d, %d, %d, '%s', '%s', %d)",
        record->game_type, record->score1, record->score2, record->m_id, record->server_id, record->charac_no, record->charac_name, record->comment_text, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WorldCupEventComment_Get(DBConnectionManager* manager, int no, WorldCupEventComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, game_type, score1, score2, m_id, server_id, charac_no, charac_name, comment_text, create_day FROM world_cup_event_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(WorldCupEventComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->game_type = row[1] ? atoi(row[1]) : 0;
    record->score1 = row[2] ? atoi(row[2]) : 0;
    record->score2 = row[3] ? atoi(row[3]) : 0;
    record->m_id = row[4] ? atoi(row[4]) : 0;
    record->server_id = row[5] ? atoi(row[5]) : 0;
    record->charac_no = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->charac_name, row[7], sizeof(record->charac_name) - 1);
    if (row[8]) strncpy(record->comment_text, row[8], sizeof(record->comment_text) - 1);
    record->create_day = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int WorldCupEventComment_GetAll(DBConnectionManager* manager, WorldCupEventComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, game_type, score1, score2, m_id, server_id, charac_no, charac_name, comment_text, create_day FROM world_cup_event_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WorldCupEventComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].game_type = row[1] ? atoi(row[1]) : 0;
        records[count].score1 = row[2] ? atoi(row[2]) : 0;
        records[count].score2 = row[3] ? atoi(row[3]) : 0;
        records[count].m_id = row[4] ? atoi(row[4]) : 0;
        records[count].server_id = row[5] ? atoi(row[5]) : 0;
        records[count].charac_no = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].charac_name, row[7], sizeof(records[count].charac_name) - 1);
        if (row[8]) strncpy(records[count].comment_text, row[8], sizeof(records[count].comment_text) - 1);
        records[count].create_day = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
