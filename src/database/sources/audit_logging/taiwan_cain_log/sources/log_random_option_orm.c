#include "log_random_option_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogRandomOption_Add(DBConnectionManager* manager, const LogRandomOption* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_random_option (level, occ_date, unseal_cnt, unseal_gold, regen_cnt, regen_gold, change_cnt, change_gold, pick_cnt, reset_cnt) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d)",
        record->level, record->occ_date, record->unseal_cnt, record->unseal_gold, record->regen_cnt, record->regen_gold, record->change_cnt, record->change_gold, record->pick_cnt, record->reset_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogRandomOption_Get(DBConnectionManager* manager, signed char level, const char* occ_date, LogRandomOption* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT level, occ_date, unseal_cnt, unseal_gold, regen_cnt, regen_gold, change_cnt, change_gold, pick_cnt, reset_cnt FROM log_random_option WHERE level = %d AND occ_date = '%s'",
        level, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogRandomOption));
    record->level = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->unseal_cnt = row[2] ? atoi(row[2]) : 0;
    record->unseal_gold = row[3] ? atoll(row[3]) : 0;
    record->regen_cnt = row[4] ? atoi(row[4]) : 0;
    record->regen_gold = row[5] ? atoll(row[5]) : 0;
    record->change_cnt = row[6] ? atoi(row[6]) : 0;
    record->change_gold = row[7] ? atoll(row[7]) : 0;
    record->pick_cnt = row[8] ? atoi(row[8]) : 0;
    record->reset_cnt = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRandomOption_GetAll(DBConnectionManager* manager, LogRandomOption* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT level, occ_date, unseal_cnt, unseal_gold, regen_cnt, regen_gold, change_cnt, change_gold, pick_cnt, reset_cnt FROM log_random_option");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogRandomOption));
        records[count].level = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].unseal_cnt = row[2] ? atoi(row[2]) : 0;
        records[count].unseal_gold = row[3] ? atoll(row[3]) : 0;
        records[count].regen_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].regen_gold = row[5] ? atoll(row[5]) : 0;
        records[count].change_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].change_gold = row[7] ? atoll(row[7]) : 0;
        records[count].pick_cnt = row[8] ? atoi(row[8]) : 0;
        records[count].reset_cnt = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
