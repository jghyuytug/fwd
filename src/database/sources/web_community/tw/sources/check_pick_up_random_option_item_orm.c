#include "check_pick_up_random_option_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CheckPickUpRandomOptionItem_Add(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO check_pick_up_random_option_item (m_id, check_count) "
        "VALUES (%d, %d)",
        record->m_id, record->check_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_Get(DBConnectionManager* manager, int m_id, CheckPickUpRandomOptionItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, check_count FROM check_pick_up_random_option_item WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CheckPickUpRandomOptionItem));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->check_count = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_GetAll(DBConnectionManager* manager, CheckPickUpRandomOptionItem* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, check_count FROM check_pick_up_random_option_item");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CheckPickUpRandomOptionItem));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].check_count = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
