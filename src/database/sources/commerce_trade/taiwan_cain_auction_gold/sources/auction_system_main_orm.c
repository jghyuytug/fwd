#include "auction_system_main_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionSystemMain_Add(DBConnectionManager* manager, const AuctionSystemMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_system_main (occ_time, regist_interval, regist_time, start_date, end_date, expire_interval, last_auction_time, expected_regist_time) "
        "VALUES ('%s', %d, '%s', '%s', '%s', %d, '%s', '%s')",
        record->occ_time, record->regist_interval, record->regist_time, record->start_date, record->end_date, record->expire_interval, record->last_auction_time, record->expected_regist_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionSystemMain_Get(DBConnectionManager* manager, int sys_auction_id, AuctionSystemMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sys_auction_id, occ_time, regist_interval, regist_time, start_date, end_date, expire_interval, last_auction_time, expected_regist_time FROM auction_system_main WHERE sys_auction_id = %d",
        sys_auction_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionSystemMain));
    record->sys_auction_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->regist_interval = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->regist_time, row[3], sizeof(record->regist_time) - 1);
    if (row[4]) strncpy(record->start_date, row[4], sizeof(record->start_date) - 1);
    if (row[5]) strncpy(record->end_date, row[5], sizeof(record->end_date) - 1);
    record->expire_interval = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->last_auction_time, row[7], sizeof(record->last_auction_time) - 1);
    if (row[8]) strncpy(record->expected_regist_time, row[8], sizeof(record->expected_regist_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionSystemMain_GetAll(DBConnectionManager* manager, AuctionSystemMain* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sys_auction_id, occ_time, regist_interval, regist_time, start_date, end_date, expire_interval, last_auction_time, expected_regist_time FROM auction_system_main");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_GOLD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionSystemMain));
        records[count].sys_auction_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].regist_interval = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].regist_time, row[3], sizeof(records[count].regist_time) - 1);
        if (row[4]) strncpy(records[count].start_date, row[4], sizeof(records[count].start_date) - 1);
        if (row[5]) strncpy(records[count].end_date, row[5], sizeof(records[count].end_date) - 1);
        records[count].expire_interval = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].last_auction_time, row[7], sizeof(records[count].last_auction_time) - 1);
        if (row[8]) strncpy(records[count].expected_regist_time, row[8], sizeof(records[count].expected_regist_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
