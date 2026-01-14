#include "charac_event_items_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacEventItems_Add(DBConnectionManager* manager, const CharacEventItems* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_event_items (charac_no, it_id, event_code, reg_time, delete_time, delete_flag, stack_count) "
        "VALUES (%d, %d, %d, '%s', '%s', %d, %d)",
        record->charac_no, record->it_id, record->event_code, record->reg_time, record->delete_time, record->delete_flag, record->stack_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacEventItems_Get(DBConnectionManager* manager, int id, CharacEventItems* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, charac_no, it_id, event_code, reg_time, delete_time, delete_flag, stack_count FROM charac_event_items WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacEventItems));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->it_id = row[2] ? atoi(row[2]) : 0;
    record->event_code = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->reg_time, row[4], sizeof(record->reg_time) - 1);
    if (row[5]) strncpy(record->delete_time, row[5], sizeof(record->delete_time) - 1);
    record->delete_flag = row[6] ? atoi(row[6]) : 0;
    record->stack_count = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacEventItems_GetAll(DBConnectionManager* manager, CharacEventItems* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, charac_no, it_id, event_code, reg_time, delete_time, delete_flag, stack_count FROM charac_event_items");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacEventItems));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].it_id = row[2] ? atoi(row[2]) : 0;
        records[count].event_code = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].reg_time, row[4], sizeof(records[count].reg_time) - 1);
        if (row[5]) strncpy(records[count].delete_time, row[5], sizeof(records[count].delete_time) - 1);
        records[count].delete_flag = row[6] ? atoi(row[6]) : 0;
        records[count].stack_count = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
