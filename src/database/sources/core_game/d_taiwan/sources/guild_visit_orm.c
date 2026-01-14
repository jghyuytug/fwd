#include "guild_visit_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild visit record */
int GuildVisit_Add(DBConnectionManager* manager, const GuildVisit* visit) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !visit) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_visit (guild_id, server_id, total_visit, today_visit) "
        "VALUES (%d, %u, %d, %d)",
        visit->guild_id, visit->server_id, visit->total_visit, visit->today_visit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild visit statistics by guild_id */
int GuildVisit_Get(DBConnectionManager* manager, int guild_id, GuildVisit* visit) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !visit) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, total_visit, today_visit FROM guild_visit "
        "WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(visit, 0, sizeof(GuildVisit));
    visit->guild_id = row[0] ? atoi(row[0]) : 0;
    visit->server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
    visit->total_visit = row[2] ? atoi(row[2]) : 0;
    visit->today_visit = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update existing guild visit statistics */
int GuildVisit_Update(DBConnectionManager* manager, const GuildVisit* visit) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !visit) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_visit SET server_id = %u, total_visit = %d, "
        "today_visit = %d WHERE guild_id = %d",
        visit->server_id, visit->total_visit, visit->today_visit, visit->guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild visit statistics by guild_id */
int GuildVisit_Delete(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_visit WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild visit record exists */
int GuildVisit_Exists(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_visit WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Increment visit counters */
int GuildVisit_IncrementVisit(DBConnectionManager* manager, int guild_id,
                               int increment_total, int increment_today) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_visit SET total_visit = total_visit + %d, "
        "today_visit = today_visit + %d WHERE guild_id = %d",
        increment_total, increment_today, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Reset today's visit count for all guilds (daily maintenance) */
int GuildVisit_ResetTodayVisits(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_visit SET today_visit = 0");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get all guild visits by server */
int GuildVisit_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                            GuildVisit* visits, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !visits || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, total_visit, today_visit FROM guild_visit "
        "WHERE server_id = %u ORDER BY total_visit DESC", server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&visits[count], 0, sizeof(GuildVisit));
        visits[count].guild_id = row[0] ? atoi(row[0]) : 0;
        visits[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        visits[count].total_visit = row[2] ? atoi(row[2]) : 0;
        visits[count].today_visit = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get top visited guilds */
int GuildVisit_GetTopVisited(DBConnectionManager* manager, int limit,
                              GuildVisit* visits, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !visits || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, total_visit, today_visit FROM guild_visit "
        "ORDER BY total_visit DESC LIMIT %d", limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < limit && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&visits[count], 0, sizeof(GuildVisit));
        visits[count].guild_id = row[0] ? atoi(row[0]) : 0;
        visits[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        visits[count].total_visit = row[2] ? atoi(row[2]) : 0;
        visits[count].today_visit = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild visit information */
void GuildVisit_PrintInfo(const GuildVisit* visit) {
    if (!visit) return;

    printf("Guild Visit Statistics:\n");
    printf("  guild_id: %d\n", visit->guild_id);
    printf("  server_id: %u\n", visit->server_id);
    printf("  total_visit: %d\n", visit->total_visit);
    printf("  today_visit: %d\n", visit->today_visit);
}
