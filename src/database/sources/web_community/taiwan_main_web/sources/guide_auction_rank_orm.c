#include "guide_auction_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideAuctionRank_Add(DBConnectionManager* manager, const GuideAuctionRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_auction_rank (server_id, rank, it_name, it_rarity, it_url) "
        "VALUES (%d, %d, '%s', %d, '%s')",
        record->server_id, record->rank, record->it_name, record->it_rarity, record->it_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideAuctionRank_Get(DBConnectionManager* manager, signed char server_id, signed char rank, GuideAuctionRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, rank, it_name, it_rarity, it_url FROM guide_auction_rank WHERE server_id = %d AND rank = %d",
        server_id, rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideAuctionRank));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->rank = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->it_name, row[2], sizeof(record->it_name) - 1);
    record->it_rarity = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->it_url, row[4], sizeof(record->it_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuideAuctionRank_GetAll(DBConnectionManager* manager, GuideAuctionRank* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, rank, it_name, it_rarity, it_url FROM guide_auction_rank");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideAuctionRank));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].rank = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].it_name, row[2], sizeof(records[count].it_name) - 1);
        records[count].it_rarity = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].it_url, row[4], sizeof(records[count].it_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
