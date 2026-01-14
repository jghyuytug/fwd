#include "guild_member_introduce_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int GuildMemberIntroduce_Add(DBConnectionManager* manager, const GuildMemberIntroduce* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_member_introduce (guild_id, charac_no, introduce) "
        "VALUES (%d, %d, '%s')",
        member->guild_id, member->charac_no, member->introduce);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMemberIntroduce* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, introduce FROM guild_member_introduce "
        "WHERE guild_id = %d AND charac_no = %d",
        guild_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(GuildMemberIntroduce));
    member->guild_id = row[0] ? atoi(row[0]) : 0;
    member->charac_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) {
        strncpy(member->introduce, row[2], sizeof(member->introduce) - 1);
        member->introduce[sizeof(member->introduce) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_Update(DBConnectionManager* manager, const GuildMemberIntroduce* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_member_introduce SET introduce = '%s' "
        "WHERE guild_id = %d AND charac_no = %d",
        member->introduce, member->guild_id, member->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_Delete(DBConnectionManager* manager, int guild_id, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_member_introduce WHERE guild_id = %d AND charac_no = %d",
        guild_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_Exists(DBConnectionManager* manager, int guild_id, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_member_introduce WHERE guild_id = %d AND charac_no = %d",
        guild_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int GuildMemberIntroduce_GetByGuild(DBConnectionManager* manager, int guild_id,
                                     GuildMemberIntroduce* members, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !members || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, introduce FROM guild_member_introduce "
        "WHERE guild_id = %d ORDER BY charac_no",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(GuildMemberIntroduce));
        members[count].guild_id = row[0] ? atoi(row[0]) : 0;
        members[count].charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(members[count].introduce, row[2], sizeof(members[count].introduce) - 1);
            members[count].introduce[sizeof(members[count].introduce) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_DeleteByGuild(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_member_introduce WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMemberIntroduce_CountByGuild(DBConnectionManager* manager, int guild_id, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_member_introduce WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *count = row[0] ? atoi(row[0]) : 0;
    } else {
        *count = 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void GuildMemberIntroduce_PrintInfo(const GuildMemberIntroduce* member) {
    if (!member) {
        printf("GuildMemberIntroduce: NULL\n");
        return;
    }

    printf("=== Guild Member Introduction ===\n");
    printf("Guild ID:    %d\n", member->guild_id);
    printf("Character #: %d\n", member->charac_no);
    printf("Introduction: %s\n", member->introduce);
    printf("================================\n");
}
