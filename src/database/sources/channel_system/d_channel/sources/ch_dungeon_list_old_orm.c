#include "ch_dungeon_list_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ChDungeonListOld_Add(DBConnectionManager* manager, const ChDungeonListOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ch_dungeon_list_old (kind_name, dungeon_id) "
        "VALUES ('%s', %d)",
        record->kind_name, record->dungeon_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChDungeonListOld_Get(DBConnectionManager* manager, short dungeon_id, ChDungeonListOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT kind_name, dungeon_id FROM ch_dungeon_list_old WHERE dungeon_id = %d",
        dungeon_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ChDungeonListOld));
    if (row[0]) strncpy(record->kind_name, row[0], sizeof(record->kind_name) - 1);
    record->dungeon_id = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ChDungeonListOld_GetAll(DBConnectionManager* manager, ChDungeonListOld* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kind_name, dungeon_id FROM ch_dungeon_list_old");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ChDungeonListOld));
        if (row[0]) strncpy(records[count].kind_name, row[0], sizeof(records[count].kind_name) - 1);
        records[count].dungeon_id = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
