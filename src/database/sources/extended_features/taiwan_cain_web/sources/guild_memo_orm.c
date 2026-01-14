#include "guild_memo_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildMemo_Add(DBConnectionManager* manager, const GuildMemo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_memo (guild_id, m_id, charac_no, charac_name, nick_name, memo, create_time, job, grow_type) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', %d, %d)",
        record->guild_id, record->m_id, record->charac_no, record->charac_name, record->nick_name, record->memo, record->create_time, record->job, record->grow_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemo_Get(DBConnectionManager* manager, int no, GuildMemo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, m_id, charac_no, charac_name, nick_name, memo, create_time, job, grow_type FROM guild_memo WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildMemo));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->guild_id = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->nick_name, row[5], sizeof(record->nick_name) - 1);
    if (row[6]) strncpy(record->memo, row[6], sizeof(record->memo) - 1);
    if (row[7]) strncpy(record->create_time, row[7], sizeof(record->create_time) - 1);
    record->job = row[8] ? atoi(row[8]) : 0;
    record->grow_type = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemo_GetAll(DBConnectionManager* manager, GuildMemo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, m_id, charac_no, charac_name, nick_name, memo, create_time, job, grow_type FROM guild_memo");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildMemo));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].guild_id = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].nick_name, row[5], sizeof(records[count].nick_name) - 1);
        if (row[6]) strncpy(records[count].memo, row[6], sizeof(records[count].memo) - 1);
        if (row[7]) strncpy(records[count].create_time, row[7], sizeof(records[count].create_time) - 1);
        records[count].job = row[8] ? atoi(row[8]) : 0;
        records[count].grow_type = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
