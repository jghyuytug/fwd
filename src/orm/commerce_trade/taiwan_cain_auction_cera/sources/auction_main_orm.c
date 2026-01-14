#include "auction_main_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionMain_Add(DBConnectionManager* manager, const AuctionMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_main (auction_id, occ_time, expire_time, owner_id, owner_name, owner_type, owner_nexon_id, buyer_id, buyer_name, price, instant_price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, black_point, unit_price, random_option, roi_high_key, roi_low_key, seperate_upgrade, item_guid) "
        "VALUES (%d, '%s', %d, %d, '%s', %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, '%s')",
        record->auction_id, record->occ_time, record->expire_time, record->owner_id, record->owner_name, record->owner_type, record->owner_nexon_id, record->buyer_id, record->buyer_name, record->price, record->instant_price, record->seal_flag, record->item_id, record->add_info, record->upgrade, record->amplify_option, record->amplify_value, record->seal_cnt, record->endurance, record->extend_info, record->black_point, record->unit_price, record->random_option, record->roi_high_key, record->roi_low_key, record->seperate_upgrade, record->item_guid);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionMain_Get(DBConnectionManager* manager, long long auction_id, AuctionMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[27];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT auction_id, occ_time, expire_time, owner_id, owner_name, owner_type, owner_nexon_id, buyer_id, buyer_name, price, instant_price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, black_point, unit_price, random_option, roi_high_key, roi_low_key, seperate_upgrade, item_guid FROM auction_main WHERE auction_id = %d",
        auction_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionMain));
    record->auction_id = row[0] ? atoll(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->expire_time = row[2] ? atoi(row[2]) : 0;
    record->owner_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->owner_name, row[4], sizeof(record->owner_name) - 1);
    record->owner_type = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->owner_nexon_id, row[6], sizeof(record->owner_nexon_id) - 1);
    record->buyer_id = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->buyer_name, row[8], sizeof(record->buyer_name) - 1);
    record->price = row[9] ? atoll(row[9]) : 0;
    record->instant_price = row[10] ? atoll(row[10]) : 0;
    record->seal_flag = row[11] ? atoi(row[11]) : 0;
    record->item_id = row[12] ? atoi(row[12]) : 0;
    record->add_info = row[13] ? atoi(row[13]) : 0;
    record->upgrade = row[14] ? atoi(row[14]) : 0;
    record->amplify_option = row[15] ? atoi(row[15]) : 0;
    record->amplify_value = row[16] ? atoi(row[16]) : 0;
    record->seal_cnt = row[17] ? atoi(row[17]) : 0;
    record->endurance = row[18] ? atoi(row[18]) : 0;
    record->extend_info = row[19] ? atoi(row[19]) : 0;
    record->black_point = row[20] ? atoi(row[20]) : 0;
    record->unit_price = row[21] ? atoi(row[21]) : 0;
    if (row[22]) strncpy(record->random_option, row[22], sizeof(record->random_option) - 1);
    record->roi_high_key = row[23] ? atoll(row[23]) : 0;
    record->roi_low_key = row[24] ? atoi(row[24]) : 0;
    record->seperate_upgrade = row[25] ? atoi(row[25]) : 0;
    if (row[26]) memcpy(record->item_guid, row[26], sizeof(record->item_guid));

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionMain_GetAll(DBConnectionManager* manager, AuctionMain* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[27];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT auction_id, occ_time, expire_time, owner_id, owner_name, owner_type, owner_nexon_id, buyer_id, buyer_name, price, instant_price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, black_point, unit_price, random_option, roi_high_key, roi_low_key, seperate_upgrade, item_guid FROM auction_main");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionMain));
        records[count].auction_id = row[0] ? atoll(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].expire_time = row[2] ? atoi(row[2]) : 0;
        records[count].owner_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].owner_name, row[4], sizeof(records[count].owner_name) - 1);
        records[count].owner_type = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].owner_nexon_id, row[6], sizeof(records[count].owner_nexon_id) - 1);
        records[count].buyer_id = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].buyer_name, row[8], sizeof(records[count].buyer_name) - 1);
        records[count].price = row[9] ? atoll(row[9]) : 0;
        records[count].instant_price = row[10] ? atoll(row[10]) : 0;
        records[count].seal_flag = row[11] ? atoi(row[11]) : 0;
        records[count].item_id = row[12] ? atoi(row[12]) : 0;
        records[count].add_info = row[13] ? atoi(row[13]) : 0;
        records[count].upgrade = row[14] ? atoi(row[14]) : 0;
        records[count].amplify_option = row[15] ? atoi(row[15]) : 0;
        records[count].amplify_value = row[16] ? atoi(row[16]) : 0;
        records[count].seal_cnt = row[17] ? atoi(row[17]) : 0;
        records[count].endurance = row[18] ? atoi(row[18]) : 0;
        records[count].extend_info = row[19] ? atoi(row[19]) : 0;
        records[count].black_point = row[20] ? atoi(row[20]) : 0;
        records[count].unit_price = row[21] ? atoi(row[21]) : 0;
        if (row[22]) strncpy(records[count].random_option, row[22], sizeof(records[count].random_option) - 1);
        records[count].roi_high_key = row[23] ? atoll(row[23]) : 0;
        records[count].roi_low_key = row[24] ? atoi(row[24]) : 0;
        records[count].seperate_upgrade = row[25] ? atoi(row[25]) : 0;
        if (row[26]) memcpy(records[count].item_guid, row[26], sizeof(records[count].item_guid));
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
