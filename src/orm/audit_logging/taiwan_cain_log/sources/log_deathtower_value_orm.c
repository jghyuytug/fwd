#include "log_deathtower_value_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDeathtowerValue_Add(DBConnectionManager* manager, const LogDeathtowerValue* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_deathtower_value (occ_date, type, level, clear_stage, recipeCnt, rareCnt, uniqCnt, card_item_goldprice, card_gold, repair_price, commonCnt, uncommonCnt, try_cnt) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->type, record->level, record->clear_stage, record->recipeCnt, record->rareCnt, record->uniqCnt, record->card_item_goldprice, record->card_gold, record->repair_price, record->commonCnt, record->uncommonCnt, record->try_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDeathtowerValue_Get(DBConnectionManager* manager, const char* occ_date, signed char type, signed char level, LogDeathtowerValue* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, type, level, clear_stage, recipeCnt, rareCnt, uniqCnt, card_item_goldprice, card_gold, repair_price, commonCnt, uncommonCnt, try_cnt FROM log_deathtower_value WHERE occ_date = '%s' AND type = %d AND level = %d",
        occ_date, type, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDeathtowerValue));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->type = row[1] ? atoi(row[1]) : 0;
    record->level = row[2] ? atoi(row[2]) : 0;
    record->clear_stage = row[3] ? atoi(row[3]) : 0;
    record->recipeCnt = row[4] ? atoi(row[4]) : 0;
    record->rareCnt = row[5] ? atoi(row[5]) : 0;
    record->uniqCnt = row[6] ? atoi(row[6]) : 0;
    record->card_item_goldprice = row[7] ? atoi(row[7]) : 0;
    record->card_gold = row[8] ? atoi(row[8]) : 0;
    record->repair_price = row[9] ? atoi(row[9]) : 0;
    record->commonCnt = row[10] ? atoi(row[10]) : 0;
    record->uncommonCnt = row[11] ? atoi(row[11]) : 0;
    record->try_cnt = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDeathtowerValue_GetAll(DBConnectionManager* manager, LogDeathtowerValue* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, type, level, clear_stage, recipeCnt, rareCnt, uniqCnt, card_item_goldprice, card_gold, repair_price, commonCnt, uncommonCnt, try_cnt FROM log_deathtower_value");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDeathtowerValue));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].type = row[1] ? atoi(row[1]) : 0;
        records[count].level = row[2] ? atoi(row[2]) : 0;
        records[count].clear_stage = row[3] ? atoi(row[3]) : 0;
        records[count].recipeCnt = row[4] ? atoi(row[4]) : 0;
        records[count].rareCnt = row[5] ? atoi(row[5]) : 0;
        records[count].uniqCnt = row[6] ? atoi(row[6]) : 0;
        records[count].card_item_goldprice = row[7] ? atoi(row[7]) : 0;
        records[count].card_gold = row[8] ? atoi(row[8]) : 0;
        records[count].repair_price = row[9] ? atoi(row[9]) : 0;
        records[count].commonCnt = row[10] ? atoi(row[10]) : 0;
        records[count].uncommonCnt = row[11] ? atoi(row[11]) : 0;
        records[count].try_cnt = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
