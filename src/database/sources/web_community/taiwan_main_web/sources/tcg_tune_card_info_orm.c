#include "tcg_tune_card_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TcgTuneCardInfo_Add(DBConnectionManager* manager, const TcgTuneCardInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tcg_tune_card_info (no, card_uninum, count, reg_date, act_info) "
        "VALUES (%d, %d, %d, %d, '%s')",
        record->no, record->card_uninum, record->count, record->reg_date, record->act_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TcgTuneCardInfo_Get(DBConnectionManager* manager, int seqidx, TcgTuneCardInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT seqidx, no, card_uninum, count, reg_date, act_info FROM tcg_tune_card_info WHERE seqidx = %d",
        seqidx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TcgTuneCardInfo));
    record->seqidx = row[0] ? atoi(row[0]) : 0;
    record->no = row[1] ? atoi(row[1]) : 0;
    record->card_uninum = row[2] ? atoi(row[2]) : 0;
    record->count = row[3] ? atoi(row[3]) : 0;
    record->reg_date = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->act_info, row[5], sizeof(record->act_info) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int TcgTuneCardInfo_GetAll(DBConnectionManager* manager, TcgTuneCardInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT seqidx, no, card_uninum, count, reg_date, act_info FROM tcg_tune_card_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TcgTuneCardInfo));
        records[count].seqidx = row[0] ? atoi(row[0]) : 0;
        records[count].no = row[1] ? atoi(row[1]) : 0;
        records[count].card_uninum = row[2] ? atoi(row[2]) : 0;
        records[count].count = row[3] ? atoi(row[3]) : 0;
        records[count].reg_date = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].act_info, row[5], sizeof(records[count].act_info) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
