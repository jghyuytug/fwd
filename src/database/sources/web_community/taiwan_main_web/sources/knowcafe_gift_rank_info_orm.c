#include "knowcafe_gift_rank_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeGiftRankInfo_Add(DBConnectionManager* manager, const KnowcafeGiftRankInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_gift_rank_info (rank, item_no, item_cnt, item_period) "
        "VALUES (%d, %d, %d, %d)",
        record->rank, record->item_no, record->item_cnt, record->item_period);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeGiftRankInfo_GetAll(DBConnectionManager* manager, KnowcafeGiftRankInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT rank, item_no, item_cnt, item_period FROM knowcafe_gift_rank_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeGiftRankInfo));
        records[count].rank = row[0] ? atoi(row[0]) : 0;
        records[count].item_no = row[1] ? atoi(row[1]) : 0;
        records[count].item_cnt = row[2] ? atoi(row[2]) : 0;
        records[count].item_period = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
