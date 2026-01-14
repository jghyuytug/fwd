#include "dnf_tower_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfTowerManager_Add(DBConnectionManager* manager, const DnfTowerManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_tower_manager (occ_time, manager_id, server_id, web_flag, game_falg) "
        "VALUES (%d, '%s', %d, %d, %d)",
        record->occ_time, record->manager_id, record->server_id, record->web_flag, record->game_falg);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTowerManager_GetAll(DBConnectionManager* manager, DnfTowerManager* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, manager_id, server_id, web_flag, game_falg FROM dnf_tower_manager");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfTowerManager));
        records[count].occ_time = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].manager_id, row[1], sizeof(records[count].manager_id) - 1);
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].web_flag = row[3] ? atoi(row[3]) : 0;
        records[count].game_falg = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
