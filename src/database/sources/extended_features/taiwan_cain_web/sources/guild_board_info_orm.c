#include "guild_board_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildBoardInfo_Add(DBConnectionManager* manager, const GuildBoardInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_board_info (guild_id, board_type, board_name, create_day, seq, modify_day, delete_day, list_auth, content_auth, write_auth, comment_flag) "
        "VALUES (%d, %d, '%s', '%s', %d, '%s', '%s', %d, %d, %d, %d)",
        record->guild_id, record->board_type, record->board_name, record->create_day, record->seq, record->modify_day, record->delete_day, record->list_auth, record->content_auth, record->write_auth, record->comment_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBoardInfo_Get(DBConnectionManager* manager, int board_id, GuildBoardInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT board_id, guild_id, board_type, board_name, create_day, seq, modify_day, delete_day, list_auth, content_auth, write_auth, comment_flag FROM guild_board_info WHERE board_id = %d",
        board_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildBoardInfo));
    record->board_id = row[0] ? atoi(row[0]) : 0;
    record->guild_id = row[1] ? atoi(row[1]) : 0;
    record->board_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->board_name, row[3], sizeof(record->board_name) - 1);
    if (row[4]) strncpy(record->create_day, row[4], sizeof(record->create_day) - 1);
    record->seq = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->modify_day, row[6], sizeof(record->modify_day) - 1);
    if (row[7]) strncpy(record->delete_day, row[7], sizeof(record->delete_day) - 1);
    record->list_auth = row[8] ? atoi(row[8]) : 0;
    record->content_auth = row[9] ? atoi(row[9]) : 0;
    record->write_auth = row[10] ? atoi(row[10]) : 0;
    record->comment_flag = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBoardInfo_GetAll(DBConnectionManager* manager, GuildBoardInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT board_id, guild_id, board_type, board_name, create_day, seq, modify_day, delete_day, list_auth, content_auth, write_auth, comment_flag FROM guild_board_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildBoardInfo));
        records[count].board_id = row[0] ? atoi(row[0]) : 0;
        records[count].guild_id = row[1] ? atoi(row[1]) : 0;
        records[count].board_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].board_name, row[3], sizeof(records[count].board_name) - 1);
        if (row[4]) strncpy(records[count].create_day, row[4], sizeof(records[count].create_day) - 1);
        records[count].seq = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].modify_day, row[6], sizeof(records[count].modify_day) - 1);
        if (row[7]) strncpy(records[count].delete_day, row[7], sizeof(records[count].delete_day) - 1);
        records[count].list_auth = row[8] ? atoi(row[8]) : 0;
        records[count].content_auth = row[9] ? atoi(row[9]) : 0;
        records[count].write_auth = row[10] ? atoi(row[10]) : 0;
        records[count].comment_flag = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
