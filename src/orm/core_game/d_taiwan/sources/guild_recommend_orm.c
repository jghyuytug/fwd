#include "guild_recommend_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild recommendation */
int GuildRecommend_Add(DBConnectionManager* manager, const GuildRecommend* recommend) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !recommend) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_recommend (no, guild_id, server_id, charac_no, "
        "charac_name, comment, recommend_time) "
        "VALUES (%d, %d, %u, %d, '%s', '%s', '%s')",
        recommend->no, recommend->guild_id, recommend->server_id,
        recommend->charac_no, recommend->charac_name, recommend->comment,
        recommend->recommend_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild recommendation by no */
int GuildRecommend_Get(DBConnectionManager* manager, int no, GuildRecommend* recommend) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !recommend) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, server_id, charac_no, charac_name, comment, "
        "recommend_time FROM guild_recommend WHERE no = %d", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(recommend, 0, sizeof(GuildRecommend));
    recommend->no = row[0] ? atoi(row[0]) : 0;
    recommend->guild_id = row[1] ? atoi(row[1]) : 0;
    recommend->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    recommend->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(recommend->charac_name, row[4], sizeof(recommend->charac_name) - 1);
    if (row[5]) strncpy(recommend->comment, row[5], sizeof(recommend->comment) - 1);
    if (row[6]) strncpy(recommend->recommend_time, row[6], sizeof(recommend->recommend_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update existing guild recommendation */
int GuildRecommend_Update(DBConnectionManager* manager, const GuildRecommend* recommend) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !recommend) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_recommend SET guild_id = %d, server_id = %u, "
        "charac_no = %d, charac_name = '%s', comment = '%s', "
        "recommend_time = '%s' WHERE no = %d",
        recommend->guild_id, recommend->server_id, recommend->charac_no,
        recommend->charac_name, recommend->comment, recommend->recommend_time,
        recommend->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild recommendation by no */
int GuildRecommend_Delete(DBConnectionManager* manager, int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_recommend WHERE no = %d", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild recommendation exists */
int GuildRecommend_Exists(DBConnectionManager* manager, int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_recommend WHERE no = %d", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all recommendations for a guild */
int GuildRecommend_GetByGuild(DBConnectionManager* manager, int guild_id,
                               GuildRecommend* recommends, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !recommends || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, server_id, charac_no, charac_name, comment, "
        "recommend_time FROM guild_recommend WHERE guild_id = %d "
        "ORDER BY recommend_time DESC", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&recommends[count], 0, sizeof(GuildRecommend));
        recommends[count].no = row[0] ? atoi(row[0]) : 0;
        recommends[count].guild_id = row[1] ? atoi(row[1]) : 0;
        recommends[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        recommends[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(recommends[count].charac_name, row[4], sizeof(recommends[count].charac_name) - 1);
        if (row[5]) strncpy(recommends[count].comment, row[5], sizeof(recommends[count].comment) - 1);
        if (row[6]) strncpy(recommends[count].recommend_time, row[6], sizeof(recommends[count].recommend_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get all recommendations by a character */
int GuildRecommend_GetByCharacter(DBConnectionManager* manager, int charac_no,
                                   GuildRecommend* recommends, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !recommends || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, guild_id, server_id, charac_no, charac_name, comment, "
        "recommend_time FROM guild_recommend WHERE charac_no = %d "
        "ORDER BY recommend_time DESC", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&recommends[count], 0, sizeof(GuildRecommend));
        recommends[count].no = row[0] ? atoi(row[0]) : 0;
        recommends[count].guild_id = row[1] ? atoi(row[1]) : 0;
        recommends[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        recommends[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(recommends[count].charac_name, row[4], sizeof(recommends[count].charac_name) - 1);
        if (row[5]) strncpy(recommends[count].comment, row[5], sizeof(recommends[count].comment) - 1);
        if (row[6]) strncpy(recommends[count].recommend_time, row[6], sizeof(recommends[count].recommend_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Delete all recommendations for a guild */
int GuildRecommend_DeleteByGuild(DBConnectionManager* manager, int guild_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_recommend WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Count recommendations for a guild */
int GuildRecommend_CountByGuild(DBConnectionManager* manager, int guild_id, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_recommend WHERE guild_id = %d", guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    *count = 0;
    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Update recommendation comment */
int GuildRecommend_UpdateComment(DBConnectionManager* manager, int no, const char* new_comment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_comment) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_recommend SET comment = '%s' WHERE no = %d",
        new_comment, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild recommendation information */
void GuildRecommend_PrintInfo(const GuildRecommend* recommend) {
    if (!recommend) return;

    printf("Guild Recommendation:\n");
    printf("  no: %d\n", recommend->no);
    printf("  guild_id: %d\n", recommend->guild_id);
    printf("  server_id: %u\n", recommend->server_id);
    printf("  charac_no: %d\n", recommend->charac_no);
    printf("  charac_name: %s\n", recommend->charac_name);
    printf("  comment: %s\n", recommend->comment);
    printf("  recommend_time: %s\n", recommend->recommend_time);
}
