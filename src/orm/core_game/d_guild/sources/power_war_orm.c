#include "power_war_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PowerWar_Add(DBConnectionManager* manager, const PowerWar* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO power_war (server_id, a_side_point, b_side_point, winner_side, occ_time) "
        "VALUES (%d, %d, %d, %d, '%s')",
        record->server_id, record->a_side_point, record->b_side_point, record->winner_side, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWar_Get(DBConnectionManager* manager, signed char server_id, PowerWar* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, a_side_point, b_side_point, winner_side, occ_time FROM power_war WHERE server_id = %d",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PowerWar));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->a_side_point = row[1] ? atoi(row[1]) : 0;
    record->b_side_point = row[2] ? atoi(row[2]) : 0;
    record->winner_side = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->occ_time, row[4], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWar_GetAll(DBConnectionManager* manager, PowerWar* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, a_side_point, b_side_point, winner_side, occ_time FROM power_war");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PowerWar));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].a_side_point = row[1] ? atoi(row[1]) : 0;
        records[count].b_side_point = row[2] ? atoi(row[2]) : 0;
        records[count].winner_side = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].occ_time, row[4], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
