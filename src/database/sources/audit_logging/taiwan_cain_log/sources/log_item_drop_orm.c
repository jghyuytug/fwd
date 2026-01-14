#include "log_item_drop_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogItemDrop_Add(DBConnectionManager* manager, const LogItemDrop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_item_drop (occ_date, channel_no, drop_type, stackable_common, stackable_uncommon, stackable_rare, stackable_unique, stackable_epic, equip_common, equip_uncommon, equip_rare, equip_unique, equip_epic, recipe_common, recipe_uncommon, recipe_rare, recipe_unique, recipe_epic, artifact_common, artifact_uncommon, artifact_rare, artifact_unique, artifact_epic, equip_chronicle, recipe_chronicle, artifact_chronicle, Stackable_chronicle) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->channel_no, record->drop_type, record->stackable_common, record->stackable_uncommon, record->stackable_rare, record->stackable_unique, record->stackable_epic, record->equip_common, record->equip_uncommon, record->equip_rare, record->equip_unique, record->equip_epic, record->recipe_common, record->recipe_uncommon, record->recipe_rare, record->recipe_unique, record->recipe_epic, record->artifact_common, record->artifact_uncommon, record->artifact_rare, record->artifact_unique, record->artifact_epic, record->equip_chronicle, record->recipe_chronicle, record->artifact_chronicle, record->Stackable_chronicle);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogItemDrop_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, signed char drop_type, LogItemDrop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[27];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, drop_type, stackable_common, stackable_uncommon, stackable_rare, stackable_unique, stackable_epic, equip_common, equip_uncommon, equip_rare, equip_unique, equip_epic, recipe_common, recipe_uncommon, recipe_rare, recipe_unique, recipe_epic, artifact_common, artifact_uncommon, artifact_rare, artifact_unique, artifact_epic, equip_chronicle, recipe_chronicle, artifact_chronicle, Stackable_chronicle FROM log_item_drop WHERE occ_date = '%s' AND channel_no = %d AND drop_type = %d",
        occ_date, channel_no, drop_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogItemDrop));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->drop_type = row[2] ? atoi(row[2]) : 0;
    record->stackable_common = row[3] ? atoi(row[3]) : 0;
    record->stackable_uncommon = row[4] ? atoi(row[4]) : 0;
    record->stackable_rare = row[5] ? atoi(row[5]) : 0;
    record->stackable_unique = row[6] ? atoi(row[6]) : 0;
    record->stackable_epic = row[7] ? atoi(row[7]) : 0;
    record->equip_common = row[8] ? atoi(row[8]) : 0;
    record->equip_uncommon = row[9] ? atoi(row[9]) : 0;
    record->equip_rare = row[10] ? atoi(row[10]) : 0;
    record->equip_unique = row[11] ? atoi(row[11]) : 0;
    record->equip_epic = row[12] ? atoi(row[12]) : 0;
    record->recipe_common = row[13] ? atoi(row[13]) : 0;
    record->recipe_uncommon = row[14] ? atoi(row[14]) : 0;
    record->recipe_rare = row[15] ? atoi(row[15]) : 0;
    record->recipe_unique = row[16] ? atoi(row[16]) : 0;
    record->recipe_epic = row[17] ? atoi(row[17]) : 0;
    record->artifact_common = row[18] ? atoi(row[18]) : 0;
    record->artifact_uncommon = row[19] ? atoi(row[19]) : 0;
    record->artifact_rare = row[20] ? atoi(row[20]) : 0;
    record->artifact_unique = row[21] ? atoi(row[21]) : 0;
    record->artifact_epic = row[22] ? atoi(row[22]) : 0;
    record->equip_chronicle = row[23] ? atoi(row[23]) : 0;
    record->recipe_chronicle = row[24] ? atoi(row[24]) : 0;
    record->artifact_chronicle = row[25] ? atoi(row[25]) : 0;
    record->Stackable_chronicle = row[26] ? atoi(row[26]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogItemDrop_GetAll(DBConnectionManager* manager, LogItemDrop* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[27];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, drop_type, stackable_common, stackable_uncommon, stackable_rare, stackable_unique, stackable_epic, equip_common, equip_uncommon, equip_rare, equip_unique, equip_epic, recipe_common, recipe_uncommon, recipe_rare, recipe_unique, recipe_epic, artifact_common, artifact_uncommon, artifact_rare, artifact_unique, artifact_epic, equip_chronicle, recipe_chronicle, artifact_chronicle, Stackable_chronicle FROM log_item_drop");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogItemDrop));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].drop_type = row[2] ? atoi(row[2]) : 0;
        records[count].stackable_common = row[3] ? atoi(row[3]) : 0;
        records[count].stackable_uncommon = row[4] ? atoi(row[4]) : 0;
        records[count].stackable_rare = row[5] ? atoi(row[5]) : 0;
        records[count].stackable_unique = row[6] ? atoi(row[6]) : 0;
        records[count].stackable_epic = row[7] ? atoi(row[7]) : 0;
        records[count].equip_common = row[8] ? atoi(row[8]) : 0;
        records[count].equip_uncommon = row[9] ? atoi(row[9]) : 0;
        records[count].equip_rare = row[10] ? atoi(row[10]) : 0;
        records[count].equip_unique = row[11] ? atoi(row[11]) : 0;
        records[count].equip_epic = row[12] ? atoi(row[12]) : 0;
        records[count].recipe_common = row[13] ? atoi(row[13]) : 0;
        records[count].recipe_uncommon = row[14] ? atoi(row[14]) : 0;
        records[count].recipe_rare = row[15] ? atoi(row[15]) : 0;
        records[count].recipe_unique = row[16] ? atoi(row[16]) : 0;
        records[count].recipe_epic = row[17] ? atoi(row[17]) : 0;
        records[count].artifact_common = row[18] ? atoi(row[18]) : 0;
        records[count].artifact_uncommon = row[19] ? atoi(row[19]) : 0;
        records[count].artifact_rare = row[20] ? atoi(row[20]) : 0;
        records[count].artifact_unique = row[21] ? atoi(row[21]) : 0;
        records[count].artifact_epic = row[22] ? atoi(row[22]) : 0;
        records[count].equip_chronicle = row[23] ? atoi(row[23]) : 0;
        records[count].recipe_chronicle = row[24] ? atoi(row[24]) : 0;
        records[count].artifact_chronicle = row[25] ? atoi(row[25]) : 0;
        records[count].Stackable_chronicle = row[26] ? atoi(row[26]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
