#include "slot_mod_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SlotModHistory_Add(DBConnectionManager* manager, const SlotModHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO slot_mod_history (mod_time, slot_part_code, slot_num, slot_position, slot1_ipg_no, slot1_rate, mod_type, mod_user_id, mod_explain) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, '%s', '%s')",
        record->mod_time, record->slot_part_code, record->slot_num, record->slot_position, record->slot1_ipg_no, record->slot1_rate, record->mod_type, record->mod_user_id, record->mod_explain);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlotModHistory_Get(DBConnectionManager* manager, const char* mod_time, short slot_part_code, signed char slot_num, signed char slot_position, SlotModHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT mod_time, slot_part_code, slot_num, slot_position, slot1_ipg_no, slot1_rate, mod_type, mod_user_id, mod_explain FROM slot_mod_history WHERE mod_time = '%s' AND slot_part_code = %d AND slot_num = %d AND slot_position = %d",
        mod_time, slot_part_code, slot_num, slot_position);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SlotModHistory));
    if (row[0]) strncpy(record->mod_time, row[0], sizeof(record->mod_time) - 1);
    record->slot_part_code = row[1] ? atoi(row[1]) : 0;
    record->slot_num = row[2] ? atoi(row[2]) : 0;
    record->slot_position = row[3] ? atoi(row[3]) : 0;
    record->slot1_ipg_no = row[4] ? atoi(row[4]) : 0;
    record->slot1_rate = row[5] ? atoi(row[5]) : 0;
    record->mod_type = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->mod_user_id, row[7], sizeof(record->mod_user_id) - 1);
    if (row[8]) strncpy(record->mod_explain, row[8], sizeof(record->mod_explain) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SlotModHistory_GetAll(DBConnectionManager* manager, SlotModHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT mod_time, slot_part_code, slot_num, slot_position, slot1_ipg_no, slot1_rate, mod_type, mod_user_id, mod_explain FROM slot_mod_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SlotModHistory));
        if (row[0]) strncpy(records[count].mod_time, row[0], sizeof(records[count].mod_time) - 1);
        records[count].slot_part_code = row[1] ? atoi(row[1]) : 0;
        records[count].slot_num = row[2] ? atoi(row[2]) : 0;
        records[count].slot_position = row[3] ? atoi(row[3]) : 0;
        records[count].slot1_ipg_no = row[4] ? atoi(row[4]) : 0;
        records[count].slot1_rate = row[5] ? atoi(row[5]) : 0;
        records[count].mod_type = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].mod_user_id, row[7], sizeof(records[count].mod_user_id) - 1);
        if (row[8]) strncpy(records[count].mod_explain, row[8], sizeof(records[count].mod_explain) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
