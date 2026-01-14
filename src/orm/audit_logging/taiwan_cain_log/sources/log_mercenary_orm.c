#include "log_mercenary_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogMercenary_Add(DBConnectionManager* manager, const LogMercenary* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_mercenary (m_id, charac_no, start_time, finish_time, type, period, area, gold, item_id, rarity) "
        "VALUES (%d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d)",
        record->m_id, record->charac_no, record->start_time, record->finish_time, record->type, record->period, record->area, record->gold, record->item_id, record->rarity);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogMercenary_GetAll(DBConnectionManager* manager, LogMercenary* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, start_time, finish_time, type, period, area, gold, item_id, rarity FROM log_mercenary");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogMercenary));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].start_time, row[2], sizeof(records[count].start_time) - 1);
        if (row[3]) strncpy(records[count].finish_time, row[3], sizeof(records[count].finish_time) - 1);
        records[count].type = row[4] ? atoi(row[4]) : 0;
        records[count].period = row[5] ? atoi(row[5]) : 0;
        records[count].area = row[6] ? atoi(row[6]) : 0;
        records[count].gold = row[7] ? atoi(row[7]) : 0;
        records[count].item_id = row[8] ? atoi(row[8]) : 0;
        records[count].rarity = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
