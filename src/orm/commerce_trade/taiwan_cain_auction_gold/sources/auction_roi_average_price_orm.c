#include "auction_roi_average_price_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionRoiAveragePrice_Add(DBConnectionManager* manager, const AuctionRoiAveragePrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_roi_average_price (item_id, upgrade, roi_high_key, roi_low_key, roi_index1, roi_index2, roi_index3, average_price, real_purchase_count, seperate_upgrade) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->item_id, record->upgrade, record->roi_high_key, record->roi_low_key, record->roi_index1, record->roi_index2, record->roi_index3, record->average_price, record->real_purchase_count, record->seperate_upgrade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionRoiAveragePrice_Get(DBConnectionManager* manager, int no, AuctionRoiAveragePrice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, item_id, upgrade, roi_high_key, roi_low_key, roi_index1, roi_index2, roi_index3, average_price, real_purchase_count, seperate_upgrade FROM auction_roi_average_price WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionRoiAveragePrice));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->item_id = row[1] ? atoi(row[1]) : 0;
    record->upgrade = row[2] ? atoi(row[2]) : 0;
    record->roi_high_key = row[3] ? atoll(row[3]) : 0;
    record->roi_low_key = row[4] ? atoi(row[4]) : 0;
    record->roi_index1 = row[5] ? atoi(row[5]) : 0;
    record->roi_index2 = row[6] ? atoi(row[6]) : 0;
    record->roi_index3 = row[7] ? atoi(row[7]) : 0;
    record->average_price = row[8] ? atoi(row[8]) : 0;
    record->real_purchase_count = row[9] ? atoi(row[9]) : 0;
    record->seperate_upgrade = row[10] ? atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionRoiAveragePrice_GetAll(DBConnectionManager* manager, AuctionRoiAveragePrice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, item_id, upgrade, roi_high_key, roi_low_key, roi_index1, roi_index2, roi_index3, average_price, real_purchase_count, seperate_upgrade FROM auction_roi_average_price");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionRoiAveragePrice));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].item_id = row[1] ? atoi(row[1]) : 0;
        records[count].upgrade = row[2] ? atoi(row[2]) : 0;
        records[count].roi_high_key = row[3] ? atoll(row[3]) : 0;
        records[count].roi_low_key = row[4] ? atoi(row[4]) : 0;
        records[count].roi_index1 = row[5] ? atoi(row[5]) : 0;
        records[count].roi_index2 = row[6] ? atoi(row[6]) : 0;
        records[count].roi_index3 = row[7] ? atoi(row[7]) : 0;
        records[count].average_price = row[8] ? atoi(row[8]) : 0;
        records[count].real_purchase_count = row[9] ? atoi(row[9]) : 0;
        records[count].seperate_upgrade = row[10] ? atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
