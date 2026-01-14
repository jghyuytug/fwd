#include "dnf_item_info_ability_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfItemInfoAbility_Add(DBConnectionManager* manager, const DnfItemInfoAbility* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_item_info_ability (it_no, ability_no, ability_name, job, skill_index, ability_sign, ability_add) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', %d)",
        record->it_no, record->ability_no, record->ability_name, record->job, record->skill_index, record->ability_sign, record->ability_add);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfoAbility_Get(DBConnectionManager* manager, int it_no, int ability_no, DnfItemInfoAbility* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, ability_no, ability_name, job, skill_index, ability_sign, ability_add FROM dnf_item_info_ability WHERE it_no = %d AND ability_no = %d",
        it_no, ability_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfItemInfoAbility));
    record->it_no = row[0] ? atoi(row[0]) : 0;
    record->ability_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->ability_name, row[2], sizeof(record->ability_name) - 1);
    record->job = row[3] ? atoi(row[3]) : 0;
    record->skill_index = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->ability_sign, row[5], sizeof(record->ability_sign) - 1);
    record->ability_add = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemInfoAbility_GetAll(DBConnectionManager* manager, DnfItemInfoAbility* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, ability_no, ability_name, job, skill_index, ability_sign, ability_add FROM dnf_item_info_ability");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfItemInfoAbility));
        records[count].it_no = row[0] ? atoi(row[0]) : 0;
        records[count].ability_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].ability_name, row[2], sizeof(records[count].ability_name) - 1);
        records[count].job = row[3] ? atoi(row[3]) : 0;
        records[count].skill_index = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].ability_sign, row[5], sizeof(records[count].ability_sign) - 1);
        records[count].ability_add = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
