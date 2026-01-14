#include "ch_dungeon_data_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ChDungeonData_Add(DBConnectionManager* manager, const ChDungeonData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ch_dungeon_data (kind_name, dungeon_name) "
        "VALUES ('%s', '%s')",
        record->kind_name, record->dungeon_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChDungeonData_GetAll(DBConnectionManager* manager, ChDungeonData* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kind_name, dungeon_name FROM ch_dungeon_data");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ChDungeonData));
        if (row[0]) strncpy(records[count].kind_name, row[0], sizeof(records[count].kind_name) - 1);
        if (row[1]) strncpy(records[count].dungeon_name, row[1], sizeof(records[count].dungeon_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
