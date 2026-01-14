#include "slot_prod_ability_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SlotProdAbilityInfo_Add(DBConnectionManager* manager, const SlotProdAbilityInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO slot_prod_ability_info (slot_part_code, slot_num, slot_position, slot_ipg_no, job, grow_type, ability_no, rate) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
        record->slot_part_code, record->slot_num, record->slot_position, record->slot_ipg_no, record->job, record->grow_type, record->ability_no, record->rate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlotProdAbilityInfo_Get(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position, int slot_ipg_no, signed char job, signed char grow_type, int ability_no, SlotProdAbilityInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_code, slot_num, slot_position, slot_ipg_no, job, grow_type, ability_no, rate FROM slot_prod_ability_info WHERE slot_part_code = %d AND slot_num = %d AND slot_position = %d AND slot_ipg_no = %d AND job = %d AND grow_type = %d AND ability_no = %d",
        slot_part_code, slot_num, slot_position, slot_ipg_no, job, grow_type, ability_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SlotProdAbilityInfo));
    record->slot_part_code = row[0] ? atoi(row[0]) : 0;
    record->slot_num = row[1] ? atoi(row[1]) : 0;
    record->slot_position = row[2] ? atoi(row[2]) : 0;
    record->slot_ipg_no = row[3] ? atoi(row[3]) : 0;
    record->job = row[4] ? atoi(row[4]) : 0;
    record->grow_type = row[5] ? atoi(row[5]) : 0;
    record->ability_no = row[6] ? atoi(row[6]) : 0;
    record->rate = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SlotProdAbilityInfo_GetAll(DBConnectionManager* manager, SlotProdAbilityInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_code, slot_num, slot_position, slot_ipg_no, job, grow_type, ability_no, rate FROM slot_prod_ability_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SlotProdAbilityInfo));
        records[count].slot_part_code = row[0] ? atoi(row[0]) : 0;
        records[count].slot_num = row[1] ? atoi(row[1]) : 0;
        records[count].slot_position = row[2] ? atoi(row[2]) : 0;
        records[count].slot_ipg_no = row[3] ? atoi(row[3]) : 0;
        records[count].job = row[4] ? atoi(row[4]) : 0;
        records[count].grow_type = row[5] ? atoi(row[5]) : 0;
        records[count].ability_no = row[6] ? atoi(row[6]) : 0;
        records[count].rate = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
