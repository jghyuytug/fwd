#include "max_count_v2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MaxCountV2_Add(DBConnectionManager* manager, const MaxCountV2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO max_count_v2 (server_info, num_occupations_charscreen, num_occupations_seriaroom, num_login_per_min, num_logout_per_min, mc_date) "
        "VALUES (%d, %d, %d, %d, %d, '%s')",
        record->server_info, record->num_occupations_charscreen, record->num_occupations_seriaroom, record->num_login_per_min, record->num_logout_per_min, record->mc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountV2_GetAll(DBConnectionManager* manager, MaxCountV2* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, num_occupations_charscreen, num_occupations_seriaroom, num_login_per_min, num_logout_per_min, mc_date FROM max_count_v2");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountV2));
        records[count].server_info = row[0] ? atoi(row[0]) : 0;
        records[count].num_occupations_charscreen = row[1] ? atoi(row[1]) : 0;
        records[count].num_occupations_seriaroom = row[2] ? atoi(row[2]) : 0;
        records[count].num_login_per_min = row[3] ? atoi(row[3]) : 0;
        records[count].num_logout_per_min = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].mc_date, row[5], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
