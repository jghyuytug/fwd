#include "slot_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SlotInfo_Add(DBConnectionManager* manager, const SlotInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO slot_info (slot_part_code, slot_num, slot_position, slot1_ipg_no, slot1_rate, regist_user_id, regist_time, mod_user_id, mod_time) "
        "VALUES (%d, %d, %d, %d, %d, '%s', '%s', '%s', '%s')",
        record->slot_part_code, record->slot_num, record->slot_position, record->slot1_ipg_no, record->slot1_rate, record->regist_user_id, record->regist_time, record->mod_user_id, record->mod_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlotInfo_Get(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position, SlotInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_code, slot_num, slot_position, slot1_ipg_no, slot1_rate, regist_user_id, regist_time, mod_user_id, mod_time FROM slot_info WHERE slot_part_code = %d AND slot_num = %d AND slot_position = %d",
        slot_part_code, slot_num, slot_position);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SlotInfo));
    record->slot_part_code = row[0] ? atoi(row[0]) : 0;
    record->slot_num = row[1] ? atoi(row[1]) : 0;
    record->slot_position = row[2] ? atoi(row[2]) : 0;
    record->slot1_ipg_no = row[3] ? atoi(row[3]) : 0;
    record->slot1_rate = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->regist_user_id, row[5], sizeof(record->regist_user_id) - 1);
    if (row[6]) strncpy(record->regist_time, row[6], sizeof(record->regist_time) - 1);
    if (row[7]) strncpy(record->mod_user_id, row[7], sizeof(record->mod_user_id) - 1);
    if (row[8]) strncpy(record->mod_time, row[8], sizeof(record->mod_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SlotInfo_GetAll(DBConnectionManager* manager, SlotInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_code, slot_num, slot_position, slot1_ipg_no, slot1_rate, regist_user_id, regist_time, mod_user_id, mod_time FROM slot_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SlotInfo));
        records[count].slot_part_code = row[0] ? atoi(row[0]) : 0;
        records[count].slot_num = row[1] ? atoi(row[1]) : 0;
        records[count].slot_position = row[2] ? atoi(row[2]) : 0;
        records[count].slot1_ipg_no = row[3] ? atoi(row[3]) : 0;
        records[count].slot1_rate = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].regist_user_id, row[5], sizeof(records[count].regist_user_id) - 1);
        if (row[6]) strncpy(records[count].regist_time, row[6], sizeof(records[count].regist_time) - 1);
        if (row[7]) strncpy(records[count].mod_user_id, row[7], sizeof(records[count].mod_user_id) - 1);
        if (row[8]) strncpy(records[count].mod_time, row[8], sizeof(records[count].mod_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
