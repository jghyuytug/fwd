#include "pvp_status_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PvpStatus_Add(DBConnectionManager* manager, const PvpStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pvp_status (occ_date, room_number, play_type, no_participants, occ_count) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_date, record->room_number, record->play_type, record->no_participants, record->occ_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpStatus_Get(DBConnectionManager* manager, const char* occ_date, signed char room_number, signed char play_type, PvpStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, room_number, play_type, no_participants, occ_count FROM pvp_status WHERE occ_date = '%s' AND room_number = %d AND play_type = %d",
        occ_date, room_number, play_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PvpStatus));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->room_number = row[1] ? atoi(row[1]) : 0;
    record->play_type = row[2] ? atoi(row[2]) : 0;
    record->no_participants = row[3] ? atoi(row[3]) : 0;
    record->occ_count = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PvpStatus_GetAll(DBConnectionManager* manager, PvpStatus* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, room_number, play_type, no_participants, occ_count FROM pvp_status");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PvpStatus));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].room_number = row[1] ? atoi(row[1]) : 0;
        records[count].play_type = row[2] ? atoi(row[2]) : 0;
        records[count].no_participants = row[3] ? atoi(row[3]) : 0;
        records[count].occ_count = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
