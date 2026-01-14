#include "avatar_select_ability_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AvatarSelectAbility_Add(DBConnectionManager* manager, const AvatarSelectAbility* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO avatar_select_ability (it_no, ability_no, ability_type, rate_change, value, job, skill_index, skill_level) "
        "VALUES (%d, %d, %d, %d, %f, %d, %d, %d)",
        record->it_no, record->ability_no, record->ability_type, record->rate_change, record->value, record->job, record->skill_index, record->skill_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AvatarSelectAbility_Get(DBConnectionManager* manager, int it_no, int ability_no, AvatarSelectAbility* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, ability_no, ability_type, rate_change, value, job, skill_index, skill_level FROM avatar_select_ability WHERE it_no = %d AND ability_no = %d",
        it_no, ability_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AvatarSelectAbility));
    record->it_no = row[0] ? atoi(row[0]) : 0;
    record->ability_no = row[1] ? atoi(row[1]) : 0;
    record->ability_type = row[2] ? atoi(row[2]) : 0;
    record->rate_change = row[3] ? atoi(row[3]) : 0;
    record->value = row[4] ? atof(row[4]) : 0;
    record->job = row[5] ? atoi(row[5]) : 0;
    record->skill_index = row[6] ? atoi(row[6]) : 0;
    record->skill_level = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AvatarSelectAbility_GetAll(DBConnectionManager* manager, AvatarSelectAbility* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, ability_no, ability_type, rate_change, value, job, skill_index, skill_level FROM avatar_select_ability");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AvatarSelectAbility));
        records[count].it_no = row[0] ? atoi(row[0]) : 0;
        records[count].ability_no = row[1] ? atoi(row[1]) : 0;
        records[count].ability_type = row[2] ? atoi(row[2]) : 0;
        records[count].rate_change = row[3] ? atoi(row[3]) : 0;
        records[count].value = row[4] ? atof(row[4]) : 0;
        records[count].job = row[5] ? atoi(row[5]) : 0;
        records[count].skill_index = row[6] ? atoi(row[6]) : 0;
        records[count].skill_level = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
