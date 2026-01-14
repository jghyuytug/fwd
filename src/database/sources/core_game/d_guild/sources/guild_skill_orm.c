#include "guild_skill_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildSkill_Add(DBConnectionManager* manager, const GuildSkill* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_skill (guild_id, remain_sp, used_sp) "
        "VALUES (%d, %d, %d)",
        record->guild_id, record->remain_sp, record->used_sp);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildSkill_Get(DBConnectionManager* manager, int guild_id, GuildSkill* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, remain_sp, used_sp FROM guild_skill WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildSkill));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->remain_sp = row[1] ? atoi(row[1]) : 0;
    record->used_sp = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildSkill_GetAll(DBConnectionManager* manager, GuildSkill* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, remain_sp, used_sp FROM guild_skill");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildSkill));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].remain_sp = row[1] ? atoi(row[1]) : 0;
        records[count].used_sp = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
