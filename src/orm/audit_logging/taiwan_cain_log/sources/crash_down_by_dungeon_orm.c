#include "crash_down_by_dungeon_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CrashDownByDungeon_Add(DBConnectionManager* manager, const CrashDownByDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO crash_down_by_dungeon (occ_time, dungeon_index, down_count) "
        "VALUES ('%s', %d, %d)",
        record->occ_time, record->dungeon_index, record->down_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CrashDownByDungeon_Get(DBConnectionManager* manager, const char* occ_time, int dungeon_index, CrashDownByDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, dungeon_index, down_count FROM crash_down_by_dungeon WHERE occ_time = '%s' AND dungeon_index = %d",
        occ_time, dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CrashDownByDungeon));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->dungeon_index = row[1] ? atoi(row[1]) : 0;
    record->down_count = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CrashDownByDungeon_GetAll(DBConnectionManager* manager, CrashDownByDungeon* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, dungeon_index, down_count FROM crash_down_by_dungeon");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CrashDownByDungeon));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].dungeon_index = row[1] ? atoi(row[1]) : 0;
        records[count].down_count = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
