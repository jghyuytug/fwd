#include "guild_halloffame_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int GuildHalloffame_Add(DBConnectionManager* manager, const GuildHalloffame* fame) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !fame) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_halloffame "
        "(fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag) "
        "VALUES (%d, %u, %d, '%s', '%s', %u, %u)",
        fame->fame_id, fame->server_id, fame->guild_id, fame->guild_name,
        fame->file_url, fame->open_flag, fame->main_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_Get(DBConnectionManager* manager, int fame_id, unsigned char server_id,
                        GuildHalloffame* fame) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !fame) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag "
        "FROM guild_halloffame "
        "WHERE fame_id = %d AND server_id = %u",
        fame_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(fame, 0, sizeof(GuildHalloffame));
    fame->fame_id = row[0] ? atoi(row[0]) : 0;
    fame->server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
    fame->guild_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(fame->guild_name, row[3], sizeof(fame->guild_name) - 1);
    if (row[4]) strncpy(fame->file_url, row[4], sizeof(fame->file_url) - 1);
    fame->open_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
    fame->main_flag = row[6] ? (unsigned char)atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_Update(DBConnectionManager* manager, const GuildHalloffame* fame) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !fame) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_halloffame "
        "SET guild_id = %d, guild_name = '%s', file_url = '%s', "
        "open_flag = %u, main_flag = %u "
        "WHERE fame_id = %d AND server_id = %u",
        fame->guild_id, fame->guild_name, fame->file_url,
        fame->open_flag, fame->main_flag,
        fame->fame_id, fame->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_Delete(DBConnectionManager* manager, int fame_id, unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_halloffame WHERE fame_id = %d AND server_id = %u",
        fame_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_Exists(DBConnectionManager* manager, int fame_id, unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_halloffame WHERE fame_id = %d AND server_id = %u",
        fame_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0 ? 1 : 0;
}

int GuildHalloffame_GetByGuild(DBConnectionManager* manager, int guild_id,
                               GuildHalloffame* fames, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !fames || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag "
        "FROM guild_halloffame "
        "WHERE guild_id = %d "
        "ORDER BY fame_id",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&fames[count], 0, sizeof(GuildHalloffame));
        fames[count].fame_id = row[0] ? atoi(row[0]) : 0;
        fames[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        fames[count].guild_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(fames[count].guild_name, row[3], sizeof(fames[count].guild_name) - 1);
        if (row[4]) strncpy(fames[count].file_url, row[4], sizeof(fames[count].file_url) - 1);
        fames[count].open_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        fames[count].main_flag = row[6] ? (unsigned char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                GuildHalloffame* fames, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !fames || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag "
        "FROM guild_halloffame "
        "WHERE server_id = %u "
        "ORDER BY fame_id",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&fames[count], 0, sizeof(GuildHalloffame));
        fames[count].fame_id = row[0] ? atoi(row[0]) : 0;
        fames[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        fames[count].guild_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(fames[count].guild_name, row[3], sizeof(fames[count].guild_name) - 1);
        if (row[4]) strncpy(fames[count].file_url, row[4], sizeof(fames[count].file_url) - 1);
        fames[count].open_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        fames[count].main_flag = row[6] ? (unsigned char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_GetByFlags(DBConnectionManager* manager, unsigned char open_flag,
                               unsigned char main_flag, GuildHalloffame* fames,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !fames || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag "
        "FROM guild_halloffame "
        "WHERE open_flag = %u AND main_flag = %u "
        "ORDER BY fame_id",
        open_flag, main_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&fames[count], 0, sizeof(GuildHalloffame));
        fames[count].fame_id = row[0] ? atoi(row[0]) : 0;
        fames[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        fames[count].guild_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(fames[count].guild_name, row[3], sizeof(fames[count].guild_name) - 1);
        if (row[4]) strncpy(fames[count].file_url, row[4], sizeof(fames[count].file_url) - 1);
        fames[count].open_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        fames[count].main_flag = row[6] ? (unsigned char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_DeleteByGuild(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_halloffame WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void GuildHalloffame_PrintInfo(const GuildHalloffame* fame) {
    if (!fame) {
        printf("GuildHalloffame: NULL\n");
        return;
    }

    printf("GuildHalloffame:\n");
    printf("  fame_id: %d\n", fame->fame_id);
    printf("  server_id: %u\n", fame->server_id);
    printf("  guild_id: %d\n", fame->guild_id);
    printf("  guild_name: %s\n", fame->guild_name);
    printf("  file_url: %s\n", fame->file_url);
    printf("  open_flag: %u\n", fame->open_flag);
    printf("  main_flag: %u\n", fame->main_flag);
}
