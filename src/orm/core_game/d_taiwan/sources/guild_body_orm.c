#include "guild_body_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 70000  /* Large enough for TEXT field */

GuildBody* GuildBody_Allocate(void) {
    GuildBody* guild = (GuildBody*)malloc(sizeof(GuildBody));
    if (guild) {
        memset(guild, 0, sizeof(GuildBody));
        guild->body = NULL;
    }
    return guild;
}

void GuildBody_Free(GuildBody* guild) {
    if (guild) {
        if (guild->body) {
            free(guild->body);
            guild->body = NULL;
        }
    }
}

int GuildBody_Add(DBConnectionManager* manager, const GuildBody* guild) {
    char* query;
    DBQueryResult result;

    if (!manager || !guild || !guild->body) return -1;

    query = (char*)malloc(MAX_QUERY_LEN);
    if (!query) return -1;

    snprintf(query, MAX_QUERY_LEN,
        "INSERT INTO guild_body (gno, body) VALUES (%d, '%s')",
        guild->gno, guild->body);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        free(query);
        return -1;
    }

    free(query);
    DBQueryResult_Free(&result);
    return 0;
}

int GuildBody_Get(DBConnectionManager* manager, int gno, GuildBody* guild) {
    char query[512];
    DBQueryResult result;
    char* row[2];

    if (!manager || !guild) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, body FROM guild_body WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(guild, 0, sizeof(GuildBody));
    guild->gno = row[0] ? atoi(row[0]) : 0;

    if (row[1]) {
        size_t len = strlen(row[1]);
        guild->body = (char*)malloc(len + 1);
        if (guild->body) {
            strcpy(guild->body, row[1]);
        }
    } else {
        guild->body = NULL;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBody_Update(DBConnectionManager* manager, const GuildBody* guild) {
    char* query;
    DBQueryResult result;

    if (!manager || !guild || !guild->body) return -1;

    query = (char*)malloc(MAX_QUERY_LEN);
    if (!query) return -1;

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE guild_body SET body = '%s' WHERE gno = %d",
        guild->body, guild->gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        free(query);
        return -1;
    }

    free(query);
    DBQueryResult_Free(&result);
    return 0;
}

int GuildBody_Delete(DBConnectionManager* manager, int gno) {
    char query[256];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_body WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBody_Exists(DBConnectionManager* manager, int gno) {
    char query[256];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_body WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

void GuildBody_PrintInfo(const GuildBody* guild) {
    if (!guild) {
        printf("GuildBody: NULL\n");
        return;
    }

    printf("=== Guild Body ===\n");
    printf("GNO:  %d\n", guild->gno);
    if (guild->body) {
        printf("Body: %s\n", guild->body);
    } else {
        printf("Body: (null)\n");
    }
    printf("==================\n");
}
