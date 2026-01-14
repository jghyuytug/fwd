#include "log_acquire_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogAcquireItem_Add(DBConnectionManager* manager, const LogAcquireItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_acquire_item (occ_date, m_id, charac_no, item_id, item_type, item_count) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_date, record->m_id, record->charac_no, record->item_id, record->item_type, record->item_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogAcquireItem_Get(DBConnectionManager* manager, const char* occ_date, int charac_no, int item_id, LogAcquireItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, charac_no, item_id, item_type, item_count FROM log_acquire_item WHERE occ_date = '%s' AND charac_no = %d AND item_id = %d",
        occ_date, charac_no, item_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogAcquireItem));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->m_id = row[1] ? atoll(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->item_id = row[3] ? atoi(row[3]) : 0;
    record->item_type = row[4] ? atoi(row[4]) : 0;
    record->item_count = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogAcquireItem_GetAll(DBConnectionManager* manager, LogAcquireItem* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, charac_no, item_id, item_type, item_count FROM log_acquire_item");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogAcquireItem));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].m_id = row[1] ? atoll(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].item_id = row[3] ? atoi(row[3]) : 0;
        records[count].item_type = row[4] ? atoi(row[4]) : 0;
        records[count].item_count = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
