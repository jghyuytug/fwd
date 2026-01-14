#include "auction_average_price_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionAveragePrice_Add(DBConnectionManager* manager, const AuctionAveragePrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_average_price (item_id, upgrade, average_price, seperate_upgrade) "
        "VALUES (%d, %d, %d, %d)",
        record->item_id, record->upgrade, record->average_price, record->seperate_upgrade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionAveragePrice_Get(DBConnectionManager* manager, int item_id, signed char upgrade, AuctionAveragePrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_id, upgrade, average_price, seperate_upgrade FROM auction_average_price WHERE item_id = %d AND upgrade = %d",
        item_id, upgrade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionAveragePrice));
    record->item_id = row[0] ? atoi(row[0]) : 0;
    record->upgrade = row[1] ? atoi(row[1]) : 0;
    record->average_price = row[2] ? atoi(row[2]) : 0;
    record->seperate_upgrade = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionAveragePrice_GetAll(DBConnectionManager* manager, AuctionAveragePrice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_id, upgrade, average_price, seperate_upgrade FROM auction_average_price");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionAveragePrice));
        records[count].item_id = row[0] ? atoi(row[0]) : 0;
        records[count].upgrade = row[1] ? atoi(row[1]) : 0;
        records[count].average_price = row[2] ? atoi(row[2]) : 0;
        records[count].seperate_upgrade = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
