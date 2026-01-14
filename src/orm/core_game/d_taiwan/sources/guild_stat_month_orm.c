#include "guild_stat_month_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

int GuildStatMonth_Add(DBConnectionManager* manager, const GuildStatMonth* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_stat_month (occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc) "
        "VALUES ('%s', %u, %u, %d, %d)",
        stat->occ_date, stat->lev, stat->server_id, stat->avg_guild_point, stat->avg_guild_point_acc);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_Get(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                       unsigned char server_id, GuildStatMonth* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !occ_date || !stat) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc "
        "FROM guild_stat_month WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(stat, 0, sizeof(GuildStatMonth));
    if (row[0]) strncpy(stat->occ_date, row[0], sizeof(stat->occ_date) - 1);
    stat->lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
    stat->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    stat->avg_guild_point = row[3] ? atoi(row[3]) : 0;
    stat->avg_guild_point_acc = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_Update(DBConnectionManager* manager, const GuildStatMonth* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_stat_month SET avg_guild_point = %d, avg_guild_point_acc = %d "
        "WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        stat->avg_guild_point, stat->avg_guild_point_acc, stat->occ_date, stat->lev, stat->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_Delete(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                          unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_stat_month WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_Exists(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                          unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_stat_month WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int GuildStatMonth_GetByDate(DBConnectionManager* manager, const char* occ_date,
                             GuildStatMonth* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !occ_date || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc "
        "FROM guild_stat_month WHERE occ_date = '%s' ORDER BY lev, server_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(GuildStatMonth));
        if (row[0]) strncpy(stats[count].occ_date, row[0], sizeof(stats[count].occ_date) - 1);
        stats[count].lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
        stats[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        stats[count].avg_guild_point = row[3] ? atoi(row[3]) : 0;
        stats[count].avg_guild_point_acc = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_GetByDateAndLevel(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                                      GuildStatMonth* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !occ_date || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc "
        "FROM guild_stat_month WHERE occ_date = '%s' AND lev = %u ORDER BY server_id",
        occ_date, lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(GuildStatMonth));
        if (row[0]) strncpy(stats[count].occ_date, row[0], sizeof(stats[count].occ_date) - 1);
        stats[count].lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
        stats[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        stats[count].avg_guild_point = row[3] ? atoi(row[3]) : 0;
        stats[count].avg_guild_point_acc = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                GuildStatMonth* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc "
        "FROM guild_stat_month WHERE server_id = %u ORDER BY occ_date DESC, lev",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(GuildStatMonth));
        if (row[0]) strncpy(stats[count].occ_date, row[0], sizeof(stats[count].occ_date) - 1);
        stats[count].lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
        stats[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        stats[count].avg_guild_point = row[3] ? atoi(row[3]) : 0;
        stats[count].avg_guild_point_acc = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_DeleteByDate(DBConnectionManager* manager, const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_stat_month WHERE occ_date = '%s'", occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void GuildStatMonth_PrintInfo(const GuildStatMonth* stat) {
    if (!stat) {
        printf("GuildStatMonth: NULL\n");
        return;
    }

    printf("=== Guild Monthly Statistics ===\n");
    printf("Date:               %s\n", stat->occ_date);
    printf("Level:              %u\n", stat->lev);
    printf("Server ID:          %u\n", stat->server_id);
    printf("Avg Guild Point:    %d\n", stat->avg_guild_point);
    printf("Avg Guild Pt (Acc): %d\n", stat->avg_guild_point_acc);
    printf("================================\n");
}
