#include "log_secret_shop_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogSecretShop_Add(DBConnectionManager* manager, const LogSecretShop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_secret_shop (npc_idx, occ_date, dungeon_idx, show_count, show_charac_count, buy_count, price) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d)",
        record->npc_idx, record->occ_date, record->dungeon_idx, record->show_count, record->show_charac_count, record->buy_count, record->price);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogSecretShop_Get(DBConnectionManager* manager, signed char npc_idx, const char* occ_date, int dungeon_idx, LogSecretShop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT npc_idx, occ_date, dungeon_idx, show_count, show_charac_count, buy_count, price FROM log_secret_shop WHERE npc_idx = %d AND occ_date = '%s' AND dungeon_idx = %d",
        npc_idx, occ_date, dungeon_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogSecretShop));
    record->npc_idx = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->dungeon_idx = row[2] ? atoi(row[2]) : 0;
    record->show_count = row[3] ? atoi(row[3]) : 0;
    record->show_charac_count = row[4] ? atoi(row[4]) : 0;
    record->buy_count = row[5] ? atoi(row[5]) : 0;
    record->price = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogSecretShop_GetAll(DBConnectionManager* manager, LogSecretShop* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT npc_idx, occ_date, dungeon_idx, show_count, show_charac_count, buy_count, price FROM log_secret_shop");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogSecretShop));
        records[count].npc_idx = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].dungeon_idx = row[2] ? atoi(row[2]) : 0;
        records[count].show_count = row[3] ? atoi(row[3]) : 0;
        records[count].show_charac_count = row[4] ? atoi(row[4]) : 0;
        records[count].buy_count = row[5] ? atoi(row[5]) : 0;
        records[count].price = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
