#include "item_part_set_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ItemPartSet_Add(DBConnectionManager* manager, const ItemPartSet* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO item_part_set (part_set_index, part_name, part_type, part_grade, part_rarity) "
        "VALUES (%d, '%s', %d, %d, %d)",
        record->part_set_index, record->part_name, record->part_type, record->part_grade, record->part_rarity);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemPartSet_Get(DBConnectionManager* manager, int idx, ItemPartSet* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, part_set_index, part_name, part_type, part_grade, part_rarity FROM item_part_set WHERE idx = %d",
        idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ItemPartSet));
    record->idx = row[0] ? atoi(row[0]) : 0;
    record->part_set_index = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->part_name, row[2], sizeof(record->part_name) - 1);
    record->part_type = row[3] ? atoi(row[3]) : 0;
    record->part_grade = row[4] ? atoi(row[4]) : 0;
    record->part_rarity = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ItemPartSet_GetAll(DBConnectionManager* manager, ItemPartSet* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, part_set_index, part_name, part_type, part_grade, part_rarity FROM item_part_set");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ItemPartSet));
        records[count].idx = row[0] ? atoi(row[0]) : 0;
        records[count].part_set_index = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].part_name, row[2], sizeof(records[count].part_name) - 1);
        records[count].part_type = row[3] ? atoi(row[3]) : 0;
        records[count].part_grade = row[4] ? atoi(row[4]) : 0;
        records[count].part_rarity = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
