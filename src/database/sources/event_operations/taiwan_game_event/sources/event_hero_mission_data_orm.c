#include "event_hero_mission_data_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventHeroMissionData_Add(DBConnectionManager* manager, const EventHeroMissionData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_hero_mission_data (server_group, charac_no, mission, mod_date) "
        "VALUES (%d, %d, '%s', '%s')",
        record->server_group, record->charac_no, record->mission, record->mod_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventHeroMissionData_Get(DBConnectionManager* manager, signed char server_group, int charac_no, EventHeroMissionData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, charac_no, mission, mod_date FROM event_hero_mission_data WHERE server_group = %d AND charac_no = %d",
        server_group, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventHeroMissionData));
    record->server_group = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->mission, row[2], sizeof(record->mission) - 1);
    if (row[3]) strncpy(record->mod_date, row[3], sizeof(record->mod_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventHeroMissionData_GetAll(DBConnectionManager* manager, EventHeroMissionData* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, charac_no, mission, mod_date FROM event_hero_mission_data");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventHeroMissionData));
        records[count].server_group = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].mission, row[2], sizeof(records[count].mission) - 1);
        if (row[3]) strncpy(records[count].mod_date, row[3], sizeof(records[count].mod_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
