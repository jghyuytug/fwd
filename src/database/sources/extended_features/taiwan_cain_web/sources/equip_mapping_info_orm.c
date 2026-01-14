#include "equip_mapping_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EquipMappingInfo_Add(DBConnectionManager* manager, const EquipMappingInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO equip_mapping_info (equip_idx, mapping_idx) "
        "VALUES (%d, %d)",
        record->equip_idx, record->mapping_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EquipMappingInfo_Get(DBConnectionManager* manager, int equip_idx, EquipMappingInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT equip_idx, mapping_idx FROM equip_mapping_info WHERE equip_idx = %d",
        equip_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EquipMappingInfo));
    record->equip_idx = row[0] ? atoi(row[0]) : 0;
    record->mapping_idx = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EquipMappingInfo_GetAll(DBConnectionManager* manager, EquipMappingInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT equip_idx, mapping_idx FROM equip_mapping_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EquipMappingInfo));
        records[count].equip_idx = row[0] ? atoi(row[0]) : 0;
        records[count].mapping_idx = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
