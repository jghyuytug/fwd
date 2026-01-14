#include "guild_join_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GuildJoinList_Add(DBConnectionManager* manager, const GuildJoinList* join) {
    if (!manager || !join) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "INSERT INTO guild_join_list (guild_id, charac_no, server_group, m_id, born_year, memo, occ_time) "
        "VALUES (%u, %u, %d, %u, '%s', '%s', '%s')",
        join->guild_id, join->charac_no, join->server_group, join->m_id,
        join->born_year, join->memo, join->occ_time);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildJoinList_Get(DBConnectionManager* manager, unsigned int guild_id, unsigned int charac_no, GuildJoinList* join) {
    if (!manager || !join) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, server_group, m_id, born_year, memo, occ_time "
        "FROM guild_join_list WHERE guild_id = %u AND charac_no = %u",
        guild_id, charac_no);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[7];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        join->guild_id = (unsigned int)atoi(row[0]);
        join->charac_no = (unsigned int)atoi(row[1]);
        join->server_group = (char)atoi(row[2]);
        join->m_id = (unsigned int)atoi(row[3]);
        strncpy(join->born_year, row[4], sizeof(join->born_year) - 1);
        join->born_year[sizeof(join->born_year) - 1] = '\0';
        strncpy(join->memo, row[5], sizeof(join->memo) - 1);
        join->memo[sizeof(join->memo) - 1] = '\0';
        strncpy(join->occ_time, row[6], sizeof(join->occ_time) - 1);
        join->occ_time[sizeof(join->occ_time) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildJoinList_Update(DBConnectionManager* manager, const GuildJoinList* join) {
    if (!manager || !join) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "UPDATE guild_join_list SET server_group = %d, m_id = %u, born_year = '%s', "
        "memo = '%s', occ_time = '%s' WHERE guild_id = %u AND charac_no = %u",
        join->server_group, join->m_id, join->born_year, join->memo, join->occ_time,
        join->guild_id, join->charac_no);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildJoinList_Delete(DBConnectionManager* manager, unsigned int guild_id, unsigned int charac_no) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM guild_join_list WHERE guild_id = %u AND charac_no = %u",
        guild_id, charac_no);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildJoinList_Exists(DBConnectionManager* manager, unsigned int guild_id, unsigned int charac_no) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_join_list WHERE guild_id = %u AND charac_no = %u",
        guild_id, charac_no);

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

int GuildJoinList_GetByGuild(DBConnectionManager* manager, unsigned int guild_id, GuildJoinList** joins, int* count) {
    if (!manager || !joins || !count) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, server_group, m_id, born_year, memo, occ_time "
        "FROM guild_join_list WHERE guild_id = %u ORDER BY occ_time DESC",
        guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[7];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *joins = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *joins = (GuildJoinList*)malloc(sizeof(GuildJoinList) * (*count));
    if (!*joins) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*joins);
        *joins = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*joins)[i].guild_id = (unsigned int)atoi(row[0]);
        (*joins)[i].charac_no = (unsigned int)atoi(row[1]);
        (*joins)[i].server_group = (char)atoi(row[2]);
        (*joins)[i].m_id = (unsigned int)atoi(row[3]);
        strncpy((*joins)[i].born_year, row[4], sizeof((*joins)[i].born_year) - 1);
        (*joins)[i].born_year[sizeof((*joins)[i].born_year) - 1] = '\0';
        strncpy((*joins)[i].memo, row[5], sizeof((*joins)[i].memo) - 1);
        (*joins)[i].memo[sizeof((*joins)[i].memo) - 1] = '\0';
        strncpy((*joins)[i].occ_time, row[6], sizeof((*joins)[i].occ_time) - 1);
        (*joins)[i].occ_time[sizeof((*joins)[i].occ_time) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildJoinList_GetByCharacter(DBConnectionManager* manager, unsigned int charac_no, GuildJoinList** joins, int* count) {
    if (!manager || !joins || !count) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, server_group, m_id, born_year, memo, occ_time "
        "FROM guild_join_list WHERE charac_no = %u ORDER BY occ_time DESC",
        charac_no);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[7];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *joins = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *joins = (GuildJoinList*)malloc(sizeof(GuildJoinList) * (*count));
    if (!*joins) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*joins);
        *joins = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*joins)[i].guild_id = (unsigned int)atoi(row[0]);
        (*joins)[i].charac_no = (unsigned int)atoi(row[1]);
        (*joins)[i].server_group = (char)atoi(row[2]);
        (*joins)[i].m_id = (unsigned int)atoi(row[3]);
        strncpy((*joins)[i].born_year, row[4], sizeof((*joins)[i].born_year) - 1);
        (*joins)[i].born_year[sizeof((*joins)[i].born_year) - 1] = '\0';
        strncpy((*joins)[i].memo, row[5], sizeof((*joins)[i].memo) - 1);
        (*joins)[i].memo[sizeof((*joins)[i].memo) - 1] = '\0';
        strncpy((*joins)[i].occ_time, row[6], sizeof((*joins)[i].occ_time) - 1);
        (*joins)[i].occ_time[sizeof((*joins)[i].occ_time) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildJoinList_GetByServer(DBConnectionManager* manager, char server_group, GuildJoinList** joins, int* count) {
    if (!manager || !joins || !count) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, server_group, m_id, born_year, memo, occ_time "
        "FROM guild_join_list WHERE server_group = %d ORDER BY occ_time DESC",
        server_group);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[7];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *joins = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *joins = (GuildJoinList*)malloc(sizeof(GuildJoinList) * (*count));
    if (!*joins) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*joins);
        *joins = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*joins)[i].guild_id = (unsigned int)atoi(row[0]);
        (*joins)[i].charac_no = (unsigned int)atoi(row[1]);
        (*joins)[i].server_group = (char)atoi(row[2]);
        (*joins)[i].m_id = (unsigned int)atoi(row[3]);
        strncpy((*joins)[i].born_year, row[4], sizeof((*joins)[i].born_year) - 1);
        (*joins)[i].born_year[sizeof((*joins)[i].born_year) - 1] = '\0';
        strncpy((*joins)[i].memo, row[5], sizeof((*joins)[i].memo) - 1);
        (*joins)[i].memo[sizeof((*joins)[i].memo) - 1] = '\0';
        strncpy((*joins)[i].occ_time, row[6], sizeof((*joins)[i].occ_time) - 1);
        (*joins)[i].occ_time[sizeof((*joins)[i].occ_time) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}
