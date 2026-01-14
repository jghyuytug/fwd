#include "auction_item_top100_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionItemTop100_Add(DBConnectionManager* manager, const AuctionItemTop100* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_item_top100 (cur_date, server_info, category, item_id, item_value) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->cur_date, record->server_info, record->category, record->item_id, record->item_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionItemTop100_Get(DBConnectionManager* manager, const char* cur_date, signed char category, int item_id, signed char server_info, AuctionItemTop100* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, server_info, category, item_id, item_value FROM auction_item_top100 WHERE cur_date = '%s' AND category = %d AND item_id = %d AND server_info = %d",
        cur_date, category, item_id, server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AuctionItemTop100));
    if (row[0]) strncpy(record->cur_date, row[0], sizeof(record->cur_date) - 1);
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->category = row[2] ? atoi(row[2]) : 0;
    record->item_id = row[3] ? atoi(row[3]) : 0;
    record->item_value = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionItemTop100_GetAll(DBConnectionManager* manager, AuctionItemTop100* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, server_info, category, item_id, item_value FROM auction_item_top100");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionItemTop100));
        if (row[0]) strncpy(records[count].cur_date, row[0], sizeof(records[count].cur_date) - 1);
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].item_id = row[3] ? atoi(row[3]) : 0;
        records[count].item_value = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
