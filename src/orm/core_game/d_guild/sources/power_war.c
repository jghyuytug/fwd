#include "power_war.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CRUD Operations

int PowerWar_Add(DBConnectionManager* manager, const PowerWar* record) {
    if (!manager || !record) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO power_war (server_id, a_side_point, b_side_point, winner_side, occ_time) "
        "VALUES (%d, %u, %u, %d, '%s')",
        record->server_id, record->a_side_point, record->b_side_point,
        record->winner_side, record->occ_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWar_Get(DBConnectionManager* manager, char server_id, PowerWar* out_record) {
    if (!manager || !out_record) return -1;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT server_id, a_side_point, b_side_point, winner_side, occ_time "
        "FROM power_war WHERE server_id = %d",
        server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    out_record->server_id = atoi(row[0]);
    out_record->a_side_point = (unsigned int)atol(row[1]);
    out_record->b_side_point = (unsigned int)atol(row[2]);
    out_record->winner_side = atoi(row[3]);
    strncpy(out_record->occ_time, row[4] ? row[4] : "0000-00-00 00:00:00", sizeof(out_record->occ_time) - 1);
    out_record->occ_time[sizeof(out_record->occ_time) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWar_Update(DBConnectionManager* manager, const PowerWar* record) {
    if (!manager || !record) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE power_war SET a_side_point = %u, b_side_point = %u, "
        "winner_side = %d, occ_time = '%s' WHERE server_id = %d",
        record->a_side_point, record->b_side_point,
        record->winner_side, record->occ_time, record->server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWar_Delete(DBConnectionManager* manager, char server_id) {
    if (!manager) return -1;

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM power_war WHERE server_id = %d", server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret >= 0 ? 0 : -1;
}

int PowerWar_Exists(DBConnectionManager* manager, char server_id) {
    if (!manager) return 0;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM power_war WHERE server_id = %d", server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* row[1];
    int exists = 0;
    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

// Business Logic

int PowerWar_GetWinnerServers(DBConnectionManager* manager, PowerWar** out_records, int* out_count) {
    if (!manager || !out_records || !out_count) return -1;

    const char* query = "SELECT server_id, a_side_point, b_side_point, winner_side, occ_time "
                       "FROM power_war WHERE winner_side != 0 ORDER BY server_id";

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Count rows first
    int capacity = 16;
    PowerWar* records = (PowerWar*)malloc(sizeof(PowerWar) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWar* new_records = (PowerWar*)realloc(records, sizeof(PowerWar) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = atoi(row[0]);
        records[count].a_side_point = (unsigned int)atol(row[1]);
        records[count].b_side_point = (unsigned int)atol(row[2]);
        records[count].winner_side = atoi(row[3]);
        strncpy(records[count].occ_time, row[4] ? row[4] : "0000-00-00 00:00:00", sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}

int PowerWar_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, PowerWar** out_records, int* out_count) {
    if (!manager || !start_date || !end_date || !out_records || !out_count) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT server_id, a_side_point, b_side_point, winner_side, occ_time "
        "FROM power_war WHERE occ_time BETWEEN '%s' AND '%s' ORDER BY occ_time DESC",
        start_date, end_date);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Dynamic allocation
    int capacity = 16;
    PowerWar* records = (PowerWar*)malloc(sizeof(PowerWar) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWar* new_records = (PowerWar*)realloc(records, sizeof(PowerWar) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = atoi(row[0]);
        records[count].a_side_point = (unsigned int)atol(row[1]);
        records[count].b_side_point = (unsigned int)atol(row[2]);
        records[count].winner_side = atoi(row[3]);
        strncpy(records[count].occ_time, row[4] ? row[4] : "0000-00-00 00:00:00", sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}

int PowerWar_GetLeaderboard(DBConnectionManager* manager, int limit, PowerWar** out_records, int* out_count) {
    if (!manager || !out_records || !out_count || limit <= 0) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT server_id, a_side_point, b_side_point, winner_side, occ_time "
        "FROM power_war ORDER BY (a_side_point + b_side_point) DESC LIMIT %d",
        limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Dynamic allocation
    int capacity = (limit > 16) ? limit : 16;
    PowerWar* records = (PowerWar*)malloc(sizeof(PowerWar) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWar* new_records = (PowerWar*)realloc(records, sizeof(PowerWar) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = atoi(row[0]);
        records[count].a_side_point = (unsigned int)atol(row[1]);
        records[count].b_side_point = (unsigned int)atol(row[2]);
        records[count].winner_side = atoi(row[3]);
        strncpy(records[count].occ_time, row[4] ? row[4] : "0000-00-00 00:00:00", sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}
