#include "log_advance_alter_layer_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogAdvanceAlterLayerStat_Add(DBConnectionManager* manager, const LogAdvanceAlterLayerStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_advance_alter_layer_stat (occ_date, item_type, item_id, item_level, use_count, use_star) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_date, record->item_type, record->item_id, record->item_level, record->use_count, record->use_star);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogAdvanceAlterLayerStat_Get(DBConnectionManager* manager, const char* occ_date, short item_type, int item_id, short item_level, LogAdvanceAlterLayerStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, item_type, item_id, item_level, use_count, use_star FROM log_advance_alter_layer_stat WHERE occ_date = '%s' AND item_type = %d AND item_id = %d AND item_level = %d",
        occ_date, item_type, item_id, item_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogAdvanceAlterLayerStat));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->item_type = row[1] ? atoi(row[1]) : 0;
    record->item_id = row[2] ? atoi(row[2]) : 0;
    record->item_level = row[3] ? atoi(row[3]) : 0;
    record->use_count = row[4] ? atoi(row[4]) : 0;
    record->use_star = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogAdvanceAlterLayerStat_GetAll(DBConnectionManager* manager, LogAdvanceAlterLayerStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, item_type, item_id, item_level, use_count, use_star FROM log_advance_alter_layer_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogAdvanceAlterLayerStat));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].item_type = row[1] ? atoi(row[1]) : 0;
        records[count].item_id = row[2] ? atoi(row[2]) : 0;
        records[count].item_level = row[3] ? atoi(row[3]) : 0;
        records[count].use_count = row[4] ? atoi(row[4]) : 0;
        records[count].use_star = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
