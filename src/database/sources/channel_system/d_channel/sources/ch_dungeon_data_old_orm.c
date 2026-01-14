#include "ch_dungeon_data_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ChDungeonDataOld_Add(DBConnectionManager* manager, const ChDungeonDataOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ch_dungeon_data_old (kind_name, dungeon_name) "
        "VALUES ('%s', '%s')",
        record->kind_name, record->dungeon_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChDungeonDataOld_Get(DBConnectionManager* manager, const char* kind_name, const char* dungeon_name, ChDungeonDataOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT kind_name, dungeon_name FROM ch_dungeon_data_old WHERE kind_name = '%s' AND dungeon_name = '%s'",
        kind_name, dungeon_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ChDungeonDataOld));
    if (row[0]) strncpy(record->kind_name, row[0], sizeof(record->kind_name) - 1);
    if (row[1]) strncpy(record->dungeon_name, row[1], sizeof(record->dungeon_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ChDungeonDataOld_GetAll(DBConnectionManager* manager, ChDungeonDataOld* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kind_name, dungeon_name FROM ch_dungeon_data_old");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ChDungeonDataOld));
        if (row[0]) strncpy(records[count].kind_name, row[0], sizeof(records[count].kind_name) - 1);
        if (row[1]) strncpy(records[count].dungeon_name, row[1], sizeof(records[count].dungeon_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
