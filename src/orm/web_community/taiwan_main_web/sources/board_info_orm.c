#include "board_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BoardInfo_Add(DBConnectionManager* manager, const BoardInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO board_info (name, class, reg_date, reg_admin) "
        "VALUES ('%s', '%s', %d, '%s')",
        record->name, record->class, record->reg_date, record->reg_admin);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BoardInfo_Get(DBConnectionManager* manager, signed char bd_id, BoardInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT bd_id, name, class, reg_date, reg_admin FROM board_info WHERE bd_id = %d",
        bd_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(BoardInfo));
    record->bd_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->name, row[1], sizeof(record->name) - 1);
    if (row[2]) strncpy(record->class, row[2], sizeof(record->class) - 1);
    record->reg_date = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->reg_admin, row[4], sizeof(record->reg_admin) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int BoardInfo_GetAll(DBConnectionManager* manager, BoardInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bd_id, name, class, reg_date, reg_admin FROM board_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BoardInfo));
        records[count].bd_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].name, row[1], sizeof(records[count].name) - 1);
        if (row[2]) strncpy(records[count].class, row[2], sizeof(records[count].class) - 1);
        records[count].reg_date = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].reg_admin, row[4], sizeof(records[count].reg_admin) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
