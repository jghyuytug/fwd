#include "guild_files_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild file (gf_no is auto-generated for each gno) */
int GuildFiles_Add(DBConnectionManager* manager, const GuildFiles* file) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !file) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_files (gno, file_server, file_location) "
        "VALUES (%d, '%s', '%s')",
        file->gno, file->file_server, file->file_location);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild file by composite primary key (gno, gf_no) */
int GuildFiles_Get(DBConnectionManager* manager, int gno, unsigned char gf_no, GuildFiles* file) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !file) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, gf_no, file_server, file_location FROM guild_files "
        "WHERE gno = %d AND gf_no = %u",
        gno, gf_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(file, 0, sizeof(GuildFiles));
    file->gno = row[0] ? atoi(row[0]) : 0;
    file->gf_no = row[1] ? (unsigned char)atoi(row[1]) : 0;
    if (row[2]) strncpy(file->file_server, row[2], sizeof(file->file_server) - 1);
    if (row[3]) strncpy(file->file_location, row[3], sizeof(file->file_location) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update existing guild file (must specify both gno and gf_no) */
int GuildFiles_Update(DBConnectionManager* manager, const GuildFiles* file) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !file) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_files SET file_server = '%s', file_location = '%s' "
        "WHERE gno = %d AND gf_no = %u",
        file->file_server, file->file_location, file->gno, file->gf_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild file by composite primary key */
int GuildFiles_Delete(DBConnectionManager* manager, int gno, unsigned char gf_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_files WHERE gno = %d AND gf_no = %u",
        gno, gf_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild file exists */
int GuildFiles_Exists(DBConnectionManager* manager, int gno, unsigned char gf_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_files WHERE gno = %d AND gf_no = %u",
        gno, gf_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all files for a guild */
int GuildFiles_GetByGuild(DBConnectionManager* manager, int gno,
                          GuildFiles* files, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !files || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, gf_no, file_server, file_location FROM guild_files "
        "WHERE gno = %d ORDER BY gf_no ASC",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&files[count], 0, sizeof(GuildFiles));
        files[count].gno = row[0] ? atoi(row[0]) : 0;
        files[count].gf_no = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(files[count].file_server, row[2], sizeof(files[count].file_server) - 1);
        if (row[3]) strncpy(files[count].file_location, row[3], sizeof(files[count].file_location) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Delete all files for a guild */
int GuildFiles_DeleteByGuild(DBConnectionManager* manager, int gno) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_files WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Count files for a guild */
int GuildFiles_CountByGuild(DBConnectionManager* manager, int gno, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_files WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    *count = 0;
    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Update file location */
int GuildFiles_UpdateLocation(DBConnectionManager* manager, int gno,
                               unsigned char gf_no, const char* new_location) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_location) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_files SET file_location = '%s' "
        "WHERE gno = %d AND gf_no = %u",
        new_location, gno, gf_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild file information */
void GuildFiles_PrintInfo(const GuildFiles* file) {
    if (!file) return;

    printf("Guild File:\n");
    printf("  gno: %d\n", file->gno);
    printf("  gf_no: %u\n", file->gf_no);
    printf("  file_server: %s\n", file->file_server);
    printf("  file_location: %s\n", file->file_location);
}
