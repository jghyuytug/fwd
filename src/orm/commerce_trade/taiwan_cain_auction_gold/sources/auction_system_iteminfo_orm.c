#include "auction_system_iteminfo_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionSystemIteminfo_Add(DBConnectionManager* manager, const AuctionSystemIteminfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_system_iteminfo (sys_auction_id, probability, price, seal_flag, item_id, add_info, upgrade, seal_cnt, endurance, extend_info) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->sys_auction_id, record->probability, record->price, record->seal_flag, record->item_id, record->add_info, record->upgrade, record->seal_cnt, record->endurance, record->extend_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionSystemIteminfo_GetAll(DBConnectionManager* manager, AuctionSystemIteminfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sys_auction_id, probability, price, seal_flag, item_id, add_info, upgrade, seal_cnt, endurance, extend_info FROM auction_system_iteminfo");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionSystemIteminfo));
        records[count].sys_auction_id = row[0] ? atoi(row[0]) : 0;
        records[count].probability = row[1] ? atoi(row[1]) : 0;
        records[count].price = row[2] ? atoi(row[2]) : 0;
        records[count].seal_flag = row[3] ? atoi(row[3]) : 0;
        records[count].item_id = row[4] ? atoi(row[4]) : 0;
        records[count].add_info = row[5] ? atoi(row[5]) : 0;
        records[count].upgrade = row[6] ? atoi(row[6]) : 0;
        records[count].seal_cnt = row[7] ? atoi(row[7]) : 0;
        records[count].endurance = row[8] ? atoi(row[8]) : 0;
        records[count].extend_info = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
