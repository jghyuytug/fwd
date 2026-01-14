#include "log_assault_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogAssault_Add(DBConnectionManager* manager, const LogAssault* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_assault (occ_date, channel_no, assault_count, load_fail_count, p2p_fail_count) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_date, record->channel_no, record->assault_count, record->load_fail_count, record->p2p_fail_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogAssault_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, LogAssault* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, assault_count, load_fail_count, p2p_fail_count FROM log_assault WHERE occ_date = '%s' AND channel_no = %d",
        occ_date, channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogAssault));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->assault_count = row[2] ? atoi(row[2]) : 0;
    record->load_fail_count = row[3] ? atoi(row[3]) : 0;
    record->p2p_fail_count = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogAssault_GetAll(DBConnectionManager* manager, LogAssault* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, assault_count, load_fail_count, p2p_fail_count FROM log_assault");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogAssault));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].assault_count = row[2] ? atoi(row[2]) : 0;
        records[count].load_fail_count = row[3] ? atoi(row[3]) : 0;
        records[count].p2p_fail_count = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
