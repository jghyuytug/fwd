#include "guild_notice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GuildNotice_Add(DBConnectionManager* manager, const GuildNotice* notice) {
    if (!manager || !notice) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "INSERT INTO guild_notice (guild_id, notice, acc_date) VALUES (%u, '%s', %d)",
        notice->guild_id, notice->notice, notice->acc_date);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildNotice_Get(DBConnectionManager* manager, unsigned int guild_id, GuildNotice* notice) {
    if (!manager || !notice) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT guild_id, notice, acc_date FROM guild_notice WHERE guild_id = %u",
        guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[3];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        notice->guild_id = (unsigned int)atoi(row[0]);
        strncpy(notice->notice, row[1], sizeof(notice->notice) - 1);
        notice->notice[sizeof(notice->notice) - 1] = '\0';
        notice->acc_date = atoi(row[2]);
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildNotice_Update(DBConnectionManager* manager, const GuildNotice* notice) {
    if (!manager || !notice) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "UPDATE guild_notice SET notice = '%s', acc_date = %d WHERE guild_id = %u",
        notice->notice, notice->acc_date, notice->guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildNotice_Delete(DBConnectionManager* manager, unsigned int guild_id) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM guild_notice WHERE guild_id = %u",
        guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildNotice_Exists(DBConnectionManager* manager, unsigned int guild_id) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_notice WHERE guild_id = %u",
        guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[1];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        ret = atoi(row[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildNotice_GetAll(DBConnectionManager* manager, GuildNotice** notices, int* count) {
    if (!manager || !notices || !count) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT guild_id, notice, acc_date FROM guild_notice ORDER BY guild_id");

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[3];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *notices = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *notices = (GuildNotice*)malloc(sizeof(GuildNotice) * (*count));
    if (!*notices) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*notices);
        *notices = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*notices)[i].guild_id = (unsigned int)atoi(row[0]);
        strncpy((*notices)[i].notice, row[1], sizeof((*notices)[i].notice) - 1);
        (*notices)[i].notice[sizeof((*notices)[i].notice) - 1] = '\0';
        (*notices)[i].acc_date = atoi(row[2]);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildNotice_GetRecentNotices(DBConnectionManager* manager, int limit, GuildNotice** notices, int* count) {
    if (!manager || !notices || !count || limit <= 0) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT guild_id, notice, acc_date FROM guild_notice ORDER BY acc_date DESC LIMIT %d",
        limit);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[3];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *notices = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *notices = (GuildNotice*)malloc(sizeof(GuildNotice) * (*count));
    if (!*notices) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*notices);
        *notices = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*notices)[i].guild_id = (unsigned int)atoi(row[0]);
        strncpy((*notices)[i].notice, row[1], sizeof((*notices)[i].notice) - 1);
        (*notices)[i].notice[sizeof((*notices)[i].notice) - 1] = '\0';
        (*notices)[i].acc_date = atoi(row[2]);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildNotice_UpdateNotice(DBConnectionManager* manager, unsigned int guild_id, const char* notice, int acc_date) {
    if (!manager || !notice) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "UPDATE guild_notice SET notice = '%s', acc_date = %d WHERE guild_id = %u",
        notice, acc_date, guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}
