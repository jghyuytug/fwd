#include "guild_introduce_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild introduction */
int GuildIntroduce_Add(DBConnectionManager* manager, const GuildIntroduce* intro) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !intro) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_introduce (guild_id, server_id, introduce) "
        "VALUES (%d, %u, '%s')",
        intro->guild_id, intro->server_id, intro->introduce);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild introduction by guild_id */
int GuildIntroduce_Get(DBConnectionManager* manager, int guild_id, GuildIntroduce* intro) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !intro) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, introduce FROM guild_introduce "
        "WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(intro, 0, sizeof(GuildIntroduce));
    intro->guild_id = row[0] ? atoi(row[0]) : 0;
    intro->server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
    if (row[2]) strncpy(intro->introduce, row[2], sizeof(intro->introduce) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update existing guild introduction */
int GuildIntroduce_Update(DBConnectionManager* manager, const GuildIntroduce* intro) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !intro) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_introduce SET server_id = %u, introduce = '%s' "
        "WHERE guild_id = %d",
        intro->server_id, intro->introduce, intro->guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild introduction by guild_id */
int GuildIntroduce_Delete(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_introduce WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild introduction exists */
int GuildIntroduce_Exists(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_introduce WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Update guild introduction text */
int GuildIntroduce_UpdateIntroduce(DBConnectionManager* manager, int guild_id,
                                    const char* new_introduce) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_introduce) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_introduce SET introduce = '%s' WHERE guild_id = %d",
        new_introduce, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get all guild introductions by server */
int GuildIntroduce_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                GuildIntroduce* intros, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !intros || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, introduce FROM guild_introduce "
        "WHERE server_id = %u ORDER BY guild_id ASC", server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&intros[count], 0, sizeof(GuildIntroduce));
        intros[count].guild_id = row[0] ? atoi(row[0]) : 0;
        intros[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(intros[count].introduce, row[2], sizeof(intros[count].introduce) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild introduction information */
void GuildIntroduce_PrintInfo(const GuildIntroduce* intro) {
    if (!intro) return;

    printf("Guild Introduction:\n");
    printf("  guild_id: %d\n", intro->guild_id);
    printf("  server_id: %u\n", intro->server_id);
    printf("  introduce: %s\n", intro->introduce);
}
