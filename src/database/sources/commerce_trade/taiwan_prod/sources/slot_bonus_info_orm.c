#include "slot_bonus_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SlotBonusInfo_Add(DBConnectionManager* manager, const SlotBonusInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO slot_bonus_info (slot_part_code, bonus_ipg_no, bonus_rate) "
        "VALUES (%d, %d, %d)",
        record->slot_part_code, record->bonus_ipg_no, record->bonus_rate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlotBonusInfo_Get(DBConnectionManager* manager, short slot_part_code, int bonus_ipg_no, SlotBonusInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_code, bonus_ipg_no, bonus_rate FROM slot_bonus_info WHERE slot_part_code = %d AND bonus_ipg_no = %d",
        slot_part_code, bonus_ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SlotBonusInfo));
    record->slot_part_code = row[0] ? atoi(row[0]) : 0;
    record->bonus_ipg_no = row[1] ? atoi(row[1]) : 0;
    record->bonus_rate = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SlotBonusInfo_GetAll(DBConnectionManager* manager, SlotBonusInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT slot_part_code, bonus_ipg_no, bonus_rate FROM slot_bonus_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SlotBonusInfo));
        records[count].slot_part_code = row[0] ? atoi(row[0]) : 0;
        records[count].bonus_ipg_no = row[1] ? atoi(row[1]) : 0;
        records[count].bonus_rate = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
