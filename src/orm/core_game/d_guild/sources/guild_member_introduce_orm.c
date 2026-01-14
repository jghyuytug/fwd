#include "guild_member_introduce_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildMemberIntroduce_Add(DBConnectionManager* manager, const GuildMemberIntroduce* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_member_introduce (guild_id, charac_no, introduce) "
        "VALUES (%d, %d, '%s')",
        record->guild_id, record->charac_no, record->introduce);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMemberIntroduce* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, introduce FROM guild_member_introduce WHERE guild_id = %d AND charac_no = %d",
        guild_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildMemberIntroduce));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->introduce, row[2], sizeof(record->introduce) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_GetAll(DBConnectionManager* manager, GuildMemberIntroduce* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, introduce FROM guild_member_introduce");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildMemberIntroduce));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].introduce, row[2], sizeof(records[count].introduce) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
