#include "auction_history_buyer_201605_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionHistoryBuyer201605_Add(DBConnectionManager* manager, const AuctionHistoryBuyer201605* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_history_buyer_201605 (auction_id, occ_time, pre_buyer_id, buyer_id, pre_price, price, pre_buyer_postal_id) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d)",
        record->auction_id, record->occ_time, record->pre_buyer_id, record->buyer_id, record->pre_price, record->price, record->pre_buyer_postal_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionHistoryBuyer201605_GetAll(DBConnectionManager* manager, AuctionHistoryBuyer201605* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT auction_id, occ_time, pre_buyer_id, buyer_id, pre_price, price, pre_buyer_postal_id FROM auction_history_buyer_201605");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionHistoryBuyer201605));
        records[count].auction_id = row[0] ? atoll(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].pre_buyer_id = row[2] ? atoi(row[2]) : 0;
        records[count].buyer_id = row[3] ? atoi(row[3]) : 0;
        records[count].pre_price = row[4] ? atoi(row[4]) : 0;
        records[count].price = row[5] ? atoi(row[5]) : 0;
        records[count].pre_buyer_postal_id = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
