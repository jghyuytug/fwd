#include "auction_manual_average_price_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionManualAveragePrice_Add(DBConnectionManager* manager, const AuctionManualAveragePrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_manual_average_price (item_id, upgrade, average_price, is_apply) "
        "VALUES (%d, %d, %d, %d)",
        record->item_id, record->upgrade, record->average_price, record->is_apply);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionManualAveragePrice_Get(DBConnectionManager* manager, int no, AuctionManualAveragePrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, item_id, upgrade, average_price, is_apply FROM auction_manual_average_price WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionManualAveragePrice));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->item_id = row[1] ? atoi(row[1]) : 0;
    record->upgrade = row[2] ? atoi(row[2]) : 0;
    record->average_price = row[3] ? atoi(row[3]) : 0;
    record->is_apply = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionManualAveragePrice_GetAll(DBConnectionManager* manager, AuctionManualAveragePrice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, item_id, upgrade, average_price, is_apply FROM auction_manual_average_price");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionManualAveragePrice));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].item_id = row[1] ? atoi(row[1]) : 0;
        records[count].upgrade = row[2] ? atoi(row[2]) : 0;
        records[count].average_price = row[3] ? atoi(row[3]) : 0;
        records[count].is_apply = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
