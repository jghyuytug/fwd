#include "inventory_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Inventory_Add(DBConnectionManager* manager, const Inventory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO inventory (charac_no, money, coin, pay_coin, event_coin, creature_flag, inventory_capacity, avatar_coin) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
        record->charac_no, record->money, record->coin, record->pay_coin, record->event_coin, record->creature_flag, record->inventory_capacity, record->avatar_coin);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Inventory_Get(DBConnectionManager* manager, int charac_no, Inventory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, money, coin, pay_coin, event_coin, creature_flag, inventory_capacity, avatar_coin FROM inventory WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Inventory));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->money = row[1] ? atoi(row[1]) : 0;
    record->coin = row[2] ? atoi(row[2]) : 0;
    record->pay_coin = row[3] ? atoi(row[3]) : 0;
    record->event_coin = row[4] ? atoi(row[4]) : 0;
    record->creature_flag = row[5] ? atoi(row[5]) : 0;
    record->inventory_capacity = row[6] ? atoi(row[6]) : 0;
    record->avatar_coin = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Inventory_GetAll(DBConnectionManager* manager, Inventory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, money, coin, pay_coin, event_coin, creature_flag, inventory_capacity, avatar_coin FROM inventory");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Inventory));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].money = row[1] ? atoi(row[1]) : 0;
        records[count].coin = row[2] ? atoi(row[2]) : 0;
        records[count].pay_coin = row[3] ? atoi(row[3]) : 0;
        records[count].event_coin = row[4] ? atoi(row[4]) : 0;
        records[count].creature_flag = row[5] ? atoi(row[5]) : 0;
        records[count].inventory_capacity = row[6] ? atoi(row[6]) : 0;
        records[count].avatar_coin = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
