#include "user_items_work_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UserItemsWork_Add(DBConnectionManager* manager, const UserItemsWork* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO user_items_work (ui_id, charac_no, slot, it_id, expire_date, obtain_from, reg_date, ipg_agency_no, ability_no, stat, clear_avatar_id, item_lock_key) "
        "VALUES (%d, %d, %d, %d, '%s', %d, '%s', '%s', %d, %d, %d, %d)",
        record->ui_id, record->charac_no, record->slot, record->it_id, record->expire_date, record->obtain_from, record->reg_date, record->ipg_agency_no, record->ability_no, record->stat, record->clear_avatar_id, record->item_lock_key);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UserItemsWork_GetAll(DBConnectionManager* manager, UserItemsWork* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ui_id, charac_no, slot, it_id, expire_date, obtain_from, reg_date, ipg_agency_no, ability_no, stat, clear_avatar_id, item_lock_key FROM user_items_work");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UserItemsWork));
        records[count].ui_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].slot = row[2] ? atoi(row[2]) : 0;
        records[count].it_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].expire_date, row[4], sizeof(records[count].expire_date) - 1);
        records[count].obtain_from = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].reg_date, row[6], sizeof(records[count].reg_date) - 1);
        if (row[7]) strncpy(records[count].ipg_agency_no, row[7], sizeof(records[count].ipg_agency_no) - 1);
        records[count].ability_no = row[8] ? atoi(row[8]) : 0;
        records[count].stat = row[9] ? atoi(row[9]) : 0;
        records[count].clear_avatar_id = row[10] ? atoi(row[10]) : 0;
        records[count].item_lock_key = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
