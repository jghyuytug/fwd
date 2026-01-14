#include "guild_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild rank record */
int GuildRank_Add(DBConnectionManager* manager, const GuildRank* rank) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !rank) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_rank (server_id, guild_id, guild_name, guild_Rank, "
        "guild_point, guild_acc_point, guild_visit, guild_acc_visit, "
        "guild_member, guild_acc_member, guild_avg_lev) "
        "VALUES (%u, %d, '%s', %u, %u, %u, %u, %u, %u, %u, %u)",
        rank->server_id, rank->guild_id, rank->guild_name, rank->guild_Rank,
        rank->guild_point, rank->guild_acc_point, rank->guild_visit,
        rank->guild_acc_visit, rank->guild_member, rank->guild_acc_member,
        rank->guild_avg_lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild rank by server_id and guild_id */
int GuildRank_Get(DBConnectionManager* manager, unsigned char server_id,
                  int guild_id, GuildRank* rank) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !rank) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, "
        "guild_acc_point, guild_visit, guild_acc_visit, guild_member, "
        "guild_acc_member, guild_avg_lev "
        "FROM guild_rank WHERE server_id = %u AND guild_id = %d",
        server_id, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(rank, 0, sizeof(GuildRank));
    rank->server_id = row[0] ? (unsigned char)atoi(row[0]) : 0;
    rank->guild_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(rank->guild_name, row[2], sizeof(rank->guild_name) - 1);
    rank->guild_Rank = row[3] ? (unsigned short)atoi(row[3]) : 0;
    rank->guild_point = row[4] ? (unsigned int)atoi(row[4]) : 0;
    rank->guild_acc_point = row[5] ? (unsigned int)atoi(row[5]) : 0;
    rank->guild_visit = row[6] ? (unsigned int)atoi(row[6]) : 0;
    rank->guild_acc_visit = row[7] ? (unsigned int)atoi(row[7]) : 0;
    rank->guild_member = row[8] ? (unsigned short)atoi(row[8]) : 0;
    rank->guild_acc_member = row[9] ? (unsigned short)atoi(row[9]) : 0;
    rank->guild_avg_lev = row[10] ? (unsigned short)atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update guild rank */
int GuildRank_Update(DBConnectionManager* manager, const GuildRank* rank) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !rank) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_rank SET guild_name = '%s', guild_Rank = %u, "
        "guild_point = %u, guild_acc_point = %u, guild_visit = %u, "
        "guild_acc_visit = %u, guild_member = %u, guild_acc_member = %u, "
        "guild_avg_lev = %u "
        "WHERE server_id = %u AND guild_id = %d",
        rank->guild_name, rank->guild_Rank, rank->guild_point,
        rank->guild_acc_point, rank->guild_visit, rank->guild_acc_visit,
        rank->guild_member, rank->guild_acc_member, rank->guild_avg_lev,
        rank->server_id, rank->guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild rank */
int GuildRank_Delete(DBConnectionManager* manager, unsigned char server_id, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_rank WHERE server_id = %u AND guild_id = %d",
        server_id, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild rank exists */
int GuildRank_Exists(DBConnectionManager* manager, unsigned char server_id, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_rank WHERE server_id = %u AND guild_id = %d",
        server_id, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all guild ranks */
int GuildRank_GetAll(DBConnectionManager* manager, GuildRank* ranks,
                     int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !ranks || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, "
        "guild_acc_point, guild_visit, guild_acc_visit, guild_member, "
        "guild_acc_member, guild_avg_lev "
        "FROM guild_rank ORDER BY server_id, guild_Rank");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&ranks[count], 0, sizeof(GuildRank));
        ranks[count].server_id = row[0] ? (unsigned char)atoi(row[0]) : 0;
        ranks[count].guild_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(ranks[count].guild_name, row[2], sizeof(ranks[count].guild_name) - 1);
        ranks[count].guild_Rank = row[3] ? (unsigned short)atoi(row[3]) : 0;
        ranks[count].guild_point = row[4] ? (unsigned int)atoi(row[4]) : 0;
        ranks[count].guild_acc_point = row[5] ? (unsigned int)atoi(row[5]) : 0;
        ranks[count].guild_visit = row[6] ? (unsigned int)atoi(row[6]) : 0;
        ranks[count].guild_acc_visit = row[7] ? (unsigned int)atoi(row[7]) : 0;
        ranks[count].guild_member = row[8] ? (unsigned short)atoi(row[8]) : 0;
        ranks[count].guild_acc_member = row[9] ? (unsigned short)atoi(row[9]) : 0;
        ranks[count].guild_avg_lev = row[10] ? (unsigned short)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild ranks by server */
int GuildRank_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                          GuildRank* ranks, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !ranks || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, "
        "guild_acc_point, guild_visit, guild_acc_visit, guild_member, "
        "guild_acc_member, guild_avg_lev "
        "FROM guild_rank WHERE server_id = %u ORDER BY guild_Rank",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&ranks[count], 0, sizeof(GuildRank));
        ranks[count].server_id = row[0] ? (unsigned char)atoi(row[0]) : 0;
        ranks[count].guild_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(ranks[count].guild_name, row[2], sizeof(ranks[count].guild_name) - 1);
        ranks[count].guild_Rank = row[3] ? (unsigned short)atoi(row[3]) : 0;
        ranks[count].guild_point = row[4] ? (unsigned int)atoi(row[4]) : 0;
        ranks[count].guild_acc_point = row[5] ? (unsigned int)atoi(row[5]) : 0;
        ranks[count].guild_visit = row[6] ? (unsigned int)atoi(row[6]) : 0;
        ranks[count].guild_acc_visit = row[7] ? (unsigned int)atoi(row[7]) : 0;
        ranks[count].guild_member = row[8] ? (unsigned short)atoi(row[8]) : 0;
        ranks[count].guild_acc_member = row[9] ? (unsigned short)atoi(row[9]) : 0;
        ranks[count].guild_avg_lev = row[10] ? (unsigned short)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get top ranked guilds by server */
int GuildRank_GetTopRanks(DBConnectionManager* manager, unsigned char server_id,
                          int limit, GuildRank* ranks, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !ranks || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, "
        "guild_acc_point, guild_visit, guild_acc_visit, guild_member, "
        "guild_acc_member, guild_avg_lev "
        "FROM guild_rank WHERE server_id = %u ORDER BY guild_Rank LIMIT %d",
        server_id, limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&ranks[count], 0, sizeof(GuildRank));
        ranks[count].server_id = row[0] ? (unsigned char)atoi(row[0]) : 0;
        ranks[count].guild_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(ranks[count].guild_name, row[2], sizeof(ranks[count].guild_name) - 1);
        ranks[count].guild_Rank = row[3] ? (unsigned short)atoi(row[3]) : 0;
        ranks[count].guild_point = row[4] ? (unsigned int)atoi(row[4]) : 0;
        ranks[count].guild_acc_point = row[5] ? (unsigned int)atoi(row[5]) : 0;
        ranks[count].guild_visit = row[6] ? (unsigned int)atoi(row[6]) : 0;
        ranks[count].guild_acc_visit = row[7] ? (unsigned int)atoi(row[7]) : 0;
        ranks[count].guild_member = row[8] ? (unsigned short)atoi(row[8]) : 0;
        ranks[count].guild_acc_member = row[9] ? (unsigned short)atoi(row[9]) : 0;
        ranks[count].guild_avg_lev = row[10] ? (unsigned short)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild ranks by point range */
int GuildRank_GetByPointRange(DBConnectionManager* manager, unsigned char server_id,
                              unsigned int min_point, unsigned int max_point,
                              GuildRank* ranks, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !ranks || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, "
        "guild_acc_point, guild_visit, guild_acc_visit, guild_member, "
        "guild_acc_member, guild_avg_lev "
        "FROM guild_rank WHERE server_id = %u AND guild_point BETWEEN %u AND %u "
        "ORDER BY guild_point DESC",
        server_id, min_point, max_point);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&ranks[count], 0, sizeof(GuildRank));
        ranks[count].server_id = row[0] ? (unsigned char)atoi(row[0]) : 0;
        ranks[count].guild_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(ranks[count].guild_name, row[2], sizeof(ranks[count].guild_name) - 1);
        ranks[count].guild_Rank = row[3] ? (unsigned short)atoi(row[3]) : 0;
        ranks[count].guild_point = row[4] ? (unsigned int)atoi(row[4]) : 0;
        ranks[count].guild_acc_point = row[5] ? (unsigned int)atoi(row[5]) : 0;
        ranks[count].guild_visit = row[6] ? (unsigned int)atoi(row[6]) : 0;
        ranks[count].guild_acc_visit = row[7] ? (unsigned int)atoi(row[7]) : 0;
        ranks[count].guild_member = row[8] ? (unsigned short)atoi(row[8]) : 0;
        ranks[count].guild_acc_member = row[9] ? (unsigned short)atoi(row[9]) : 0;
        ranks[count].guild_avg_lev = row[10] ? (unsigned short)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild rank information */
void GuildRank_PrintInfo(const GuildRank* rank) {
    if (!rank) return;

    printf("Guild Rank Info:\n");
    printf("  Server ID: %u\n", rank->server_id);
    printf("  Guild ID: %d\n", rank->guild_id);
    printf("  Guild Name: %s\n", rank->guild_name);
    printf("  Rank: %u\n", rank->guild_Rank);
    printf("  Points: %u (Acc: %u)\n", rank->guild_point, rank->guild_acc_point);
    printf("  Visits: %u (Acc: %u)\n", rank->guild_visit, rank->guild_acc_visit);
    printf("  Members: %u (Acc: %u)\n", rank->guild_member, rank->guild_acc_member);
    printf("  Average Level: %u\n", rank->guild_avg_lev);
}
