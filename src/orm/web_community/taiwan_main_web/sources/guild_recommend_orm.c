#include "guild_recommend_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildRecommend_Add(DBConnectionManager* manager, const GuildRecommend* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_recommend (no, guild_id, server_id, charac_no, charac_name, comment, recommend_time) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', '%s')",
        record->no, record->guild_id, record->server_id, record->charac_no, record->charac_name, record->comment, record->recommend_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildRecommend_Get(DBConnectionManager* manager, int no, GuildRecommend* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, server_id, charac_no, charac_name, comment, recommend_time FROM guild_recommend WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildRecommend));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->guild_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->comment, row[5], sizeof(record->comment) - 1);
    if (row[6]) strncpy(record->recommend_time, row[6], sizeof(record->recommend_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildRecommend_GetAll(DBConnectionManager* manager, GuildRecommend* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, server_id, charac_no, charac_name, comment, recommend_time FROM guild_recommend");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildRecommend));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].guild_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].comment, row[5], sizeof(records[count].comment) - 1);
        if (row[6]) strncpy(records[count].recommend_time, row[6], sizeof(records[count].recommend_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
