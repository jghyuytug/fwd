#include "game_genre_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GameGenre_Add(DBConnectionManager* manager, const GameGenre* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO game_genre (jenre_name) "
        "VALUES ('%s')",
        record->jenre_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GameGenre_Get(DBConnectionManager* manager, signed char genre_code, GameGenre* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT genre_code, jenre_name FROM game_genre WHERE genre_code = %d",
        genre_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GameGenre));
    record->genre_code = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->jenre_name, row[1], sizeof(record->jenre_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GameGenre_GetAll(DBConnectionManager* manager, GameGenre* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT genre_code, jenre_name FROM game_genre");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GameGenre));
        records[count].genre_code = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].jenre_name, row[1], sizeof(records[count].jenre_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
