#include "auction_history_201604_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionHistory201604_Add(DBConnectionManager* manager, const AuctionHistory201604* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_history_201604 (auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, owner_postal_id, buyer_postal_id, unit_price) "
        "VALUES (%d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->auction_id, record->start_time, record->occ_time, record->event_type, record->owner_id, record->buyer_id, record->price, record->seal_flag, record->item_id, record->add_info, record->upgrade, record->amplify_option, record->amplify_value, record->seal_cnt, record->endurance, record->extend_info, record->owner_postal_id, record->buyer_postal_id, record->unit_price);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionHistory201604_Get(DBConnectionManager* manager, long long auction_id, AuctionHistory201604* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, owner_postal_id, buyer_postal_id, unit_price FROM auction_history_201604 WHERE auction_id = %d",
        auction_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionHistory201604));
    record->auction_id = row[0] ? atoll(row[0]) : 0;
    if (row[1]) strncpy(record->start_time, row[1], sizeof(record->start_time) - 1);
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);
    record->event_type = row[3] ? atoi(row[3]) : 0;
    record->owner_id = row[4] ? atoi(row[4]) : 0;
    record->buyer_id = row[5] ? atoi(row[5]) : 0;
    record->price = row[6] ? atoi(row[6]) : 0;
    record->seal_flag = row[7] ? atoi(row[7]) : 0;
    record->item_id = row[8] ? atoi(row[8]) : 0;
    record->add_info = row[9] ? atoi(row[9]) : 0;
    record->upgrade = row[10] ? atoi(row[10]) : 0;
    record->amplify_option = row[11] ? atoi(row[11]) : 0;
    record->amplify_value = row[12] ? atoi(row[12]) : 0;
    record->seal_cnt = row[13] ? atoi(row[13]) : 0;
    record->endurance = row[14] ? atoi(row[14]) : 0;
    record->extend_info = row[15] ? atoi(row[15]) : 0;
    record->owner_postal_id = row[16] ? atoi(row[16]) : 0;
    record->buyer_postal_id = row[17] ? atoi(row[17]) : 0;
    record->unit_price = row[18] ? atoi(row[18]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionHistory201604_GetAll(DBConnectionManager* manager, AuctionHistory201604* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, owner_postal_id, buyer_postal_id, unit_price FROM auction_history_201604");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionHistory201604));
        records[count].auction_id = row[0] ? atoll(row[0]) : 0;
        if (row[1]) strncpy(records[count].start_time, row[1], sizeof(records[count].start_time) - 1);
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        records[count].event_type = row[3] ? atoi(row[3]) : 0;
        records[count].owner_id = row[4] ? atoi(row[4]) : 0;
        records[count].buyer_id = row[5] ? atoi(row[5]) : 0;
        records[count].price = row[6] ? atoi(row[6]) : 0;
        records[count].seal_flag = row[7] ? atoi(row[7]) : 0;
        records[count].item_id = row[8] ? atoi(row[8]) : 0;
        records[count].add_info = row[9] ? atoi(row[9]) : 0;
        records[count].upgrade = row[10] ? atoi(row[10]) : 0;
        records[count].amplify_option = row[11] ? atoi(row[11]) : 0;
        records[count].amplify_value = row[12] ? atoi(row[12]) : 0;
        records[count].seal_cnt = row[13] ? atoi(row[13]) : 0;
        records[count].endurance = row[14] ? atoi(row[14]) : 0;
        records[count].extend_info = row[15] ? atoi(row[15]) : 0;
        records[count].owner_postal_id = row[16] ? atoi(row[16]) : 0;
        records[count].buyer_postal_id = row[17] ? atoi(row[17]) : 0;
        records[count].unit_price = row[18] ? atoi(row[18]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
