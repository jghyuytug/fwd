#include "combo_skill_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ComboSkill_Add(DBConnectionManager* manager, const ComboSkill* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO combo_skill (charac_no, combo_idx, value1, value2, value3, value4, value5, value6) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
        record->charac_no, record->combo_idx, record->value1, record->value2, record->value3, record->value4, record->value5, record->value6);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ComboSkill_Get(DBConnectionManager* manager, int charac_no, int combo_idx, ComboSkill* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, combo_idx, value1, value2, value3, value4, value5, value6 FROM combo_skill WHERE charac_no = %d AND combo_idx = %d",
        charac_no, combo_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ComboSkill));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->combo_idx = row[1] ? atoi(row[1]) : 0;
    record->value1 = row[2] ? atoi(row[2]) : 0;
    record->value2 = row[3] ? atoi(row[3]) : 0;
    record->value3 = row[4] ? atoi(row[4]) : 0;
    record->value4 = row[5] ? atoi(row[5]) : 0;
    record->value5 = row[6] ? atoi(row[6]) : 0;
    record->value6 = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ComboSkill_GetAll(DBConnectionManager* manager, ComboSkill* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, combo_idx, value1, value2, value3, value4, value5, value6 FROM combo_skill");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ComboSkill));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].combo_idx = row[1] ? atoi(row[1]) : 0;
        records[count].value1 = row[2] ? atoi(row[2]) : 0;
        records[count].value2 = row[3] ? atoi(row[3]) : 0;
        records[count].value3 = row[4] ? atoi(row[4]) : 0;
        records[count].value4 = row[5] ? atoi(row[5]) : 0;
        records[count].value5 = row[6] ? atoi(row[6]) : 0;
        records[count].value6 = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
