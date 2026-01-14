#include "user_items_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UserItemsHistory_Add(DBConnectionManager* manager, const UserItemsHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO user_items_history (charac_no, slot, it_id, expire_date, obtain_from, reg_date, ipg_agency_no, ability_no, stat, clear_avatar_id, item_lock_key, to_ipg_agency_no, m_time, hidden_option, emblem_endurance, color1, color2, trade_restrict) "
        "VALUES (%d, %d, %d, '%s', %d, '%s', '%s', %d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d)",
        record->charac_no, record->slot, record->it_id, record->expire_date, record->obtain_from, record->reg_date, record->ipg_agency_no, record->ability_no, record->stat, record->clear_avatar_id, record->item_lock_key, record->to_ipg_agency_no, record->m_time, record->hidden_option, record->emblem_endurance, record->color1, record->color2, record->trade_restrict);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UserItemsHistory_Get(DBConnectionManager* manager, int ui_id, UserItemsHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ui_id, charac_no, slot, it_id, expire_date, obtain_from, reg_date, ipg_agency_no, ability_no, stat, clear_avatar_id, item_lock_key, to_ipg_agency_no, m_time, hidden_option, emblem_endurance, color1, color2, trade_restrict FROM user_items_history WHERE ui_id = %d",
        ui_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(UserItemsHistory));
    record->ui_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->slot = row[2] ? atoi(row[2]) : 0;
    record->it_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->expire_date, row[4], sizeof(record->expire_date) - 1);
    record->obtain_from = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->reg_date, row[6], sizeof(record->reg_date) - 1);
    if (row[7]) strncpy(record->ipg_agency_no, row[7], sizeof(record->ipg_agency_no) - 1);
    record->ability_no = row[8] ? atoi(row[8]) : 0;
    record->stat = row[9] ? atoi(row[9]) : 0;
    record->clear_avatar_id = row[10] ? atoi(row[10]) : 0;
    record->item_lock_key = row[11] ? atoi(row[11]) : 0;
    if (row[12]) strncpy(record->to_ipg_agency_no, row[12], sizeof(record->to_ipg_agency_no) - 1);
    if (row[13]) strncpy(record->m_time, row[13], sizeof(record->m_time) - 1);
    record->hidden_option = row[14] ? atoi(row[14]) : 0;
    record->emblem_endurance = row[15] ? atoi(row[15]) : 0;
    record->color1 = row[16] ? atoi(row[16]) : 0;
    record->color2 = row[17] ? atoi(row[17]) : 0;
    record->trade_restrict = row[18] ? atoi(row[18]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int UserItemsHistory_GetAll(DBConnectionManager* manager, UserItemsHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ui_id, charac_no, slot, it_id, expire_date, obtain_from, reg_date, ipg_agency_no, ability_no, stat, clear_avatar_id, item_lock_key, to_ipg_agency_no, m_time, hidden_option, emblem_endurance, color1, color2, trade_restrict FROM user_items_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UserItemsHistory));
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
        if (row[12]) strncpy(records[count].to_ipg_agency_no, row[12], sizeof(records[count].to_ipg_agency_no) - 1);
        if (row[13]) strncpy(records[count].m_time, row[13], sizeof(records[count].m_time) - 1);
        records[count].hidden_option = row[14] ? atoi(row[14]) : 0;
        records[count].emblem_endurance = row[15] ? atoi(row[15]) : 0;
        records[count].color1 = row[16] ? atoi(row[16]) : 0;
        records[count].color2 = row[17] ? atoi(row[17]) : 0;
        records[count].trade_restrict = row[18] ? atoi(row[18]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
