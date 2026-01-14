#include "log_cerashop_gift_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogCerashopGift_Add(DBConnectionManager* manager, const LogCerashopGift* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_cerashop_gift (occ_time, from_mid, from_charac_no, to_mid, to_charac_no, item_idx, item_cnt) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->from_mid, record->from_charac_no, record->to_mid, record->to_charac_no, record->item_idx, record->item_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogCerashopGift_Get(DBConnectionManager* manager, int idx, LogCerashopGift* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, occ_time, from_mid, from_charac_no, to_mid, to_charac_no, item_idx, item_cnt FROM log_cerashop_gift WHERE idx = %d",
        idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogCerashopGift));
    record->idx = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->from_mid = row[2] ? atoi(row[2]) : 0;
    record->from_charac_no = row[3] ? atoi(row[3]) : 0;
    record->to_mid = row[4] ? atoi(row[4]) : 0;
    record->to_charac_no = row[5] ? atoi(row[5]) : 0;
    record->item_idx = row[6] ? atoi(row[6]) : 0;
    record->item_cnt = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogCerashopGift_GetAll(DBConnectionManager* manager, LogCerashopGift* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, occ_time, from_mid, from_charac_no, to_mid, to_charac_no, item_idx, item_cnt FROM log_cerashop_gift");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogCerashopGift));
        records[count].idx = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].from_mid = row[2] ? atoi(row[2]) : 0;
        records[count].from_charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].to_mid = row[4] ? atoi(row[4]) : 0;
        records[count].to_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].item_idx = row[6] ? atoi(row[6]) : 0;
        records[count].item_cnt = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
