#include "slot_master_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SlotMaster_Add(DBConnectionManager* manager, const SlotMaster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO slot_master (slot_part_type, slot_part_code, slot_part_name, slot_part_step, slot_part_script, slot_npc_no, slot_sell_item_type, slot_set_flag, slot_price, slot_bonus_rate, slot_it_no, slot_it_cnt, slot_stat) "
        "VALUES (%d, %d, '%s', %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d)",
        record->slot_part_type, record->slot_part_code, record->slot_part_name, record->slot_part_step, record->slot_part_script, record->slot_npc_no, record->slot_sell_item_type, record->slot_set_flag, record->slot_price, record->slot_bonus_rate, record->slot_it_no, record->slot_it_cnt, record->slot_stat);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlotMaster_Get(DBConnectionManager* manager, short slot_part_code, SlotMaster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_type, slot_part_code, slot_part_name, slot_part_step, slot_part_script, slot_npc_no, slot_sell_item_type, slot_set_flag, slot_price, slot_bonus_rate, slot_it_no, slot_it_cnt, slot_stat FROM slot_master WHERE slot_part_code = %d",
        slot_part_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SlotMaster));
    record->slot_part_type = row[0] ? atoi(row[0]) : 0;
    record->slot_part_code = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->slot_part_name, row[2], sizeof(record->slot_part_name) - 1);
    record->slot_part_step = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->slot_part_script, row[4], sizeof(record->slot_part_script) - 1);
    record->slot_npc_no = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->slot_sell_item_type, row[6], sizeof(record->slot_sell_item_type) - 1);
    record->slot_set_flag = row[7] ? atoi(row[7]) : 0;
    record->slot_price = row[8] ? atoi(row[8]) : 0;
    record->slot_bonus_rate = row[9] ? atoi(row[9]) : 0;
    record->slot_it_no = row[10] ? atoi(row[10]) : 0;
    record->slot_it_cnt = row[11] ? atoi(row[11]) : 0;
    record->slot_stat = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SlotMaster_GetAll(DBConnectionManager* manager, SlotMaster* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_type, slot_part_code, slot_part_name, slot_part_step, slot_part_script, slot_npc_no, slot_sell_item_type, slot_set_flag, slot_price, slot_bonus_rate, slot_it_no, slot_it_cnt, slot_stat FROM slot_master");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SlotMaster));
        records[count].slot_part_type = row[0] ? atoi(row[0]) : 0;
        records[count].slot_part_code = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].slot_part_name, row[2], sizeof(records[count].slot_part_name) - 1);
        records[count].slot_part_step = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].slot_part_script, row[4], sizeof(records[count].slot_part_script) - 1);
        records[count].slot_npc_no = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].slot_sell_item_type, row[6], sizeof(records[count].slot_sell_item_type) - 1);
        records[count].slot_set_flag = row[7] ? atoi(row[7]) : 0;
        records[count].slot_price = row[8] ? atoi(row[8]) : 0;
        records[count].slot_bonus_rate = row[9] ? atoi(row[9]) : 0;
        records[count].slot_it_no = row[10] ? atoi(row[10]) : 0;
        records[count].slot_it_cnt = row[11] ? atoi(row[11]) : 0;
        records[count].slot_stat = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
