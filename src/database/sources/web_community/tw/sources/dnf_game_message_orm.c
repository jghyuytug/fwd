#include "dnf_game_message_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfGameMessage_Add(DBConnectionManager* manager, const DnfGameMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_game_message (message, display_type, start_h, end_h, occ_date) "
        "VALUES ('%s', %d, %d, %d, '%s')",
        record->message, record->display_type, record->start_h, record->end_h, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Get(DBConnectionManager* manager, int no, DnfGameMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date FROM dnf_game_message WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfGameMessage));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->message, row[1], sizeof(record->message) - 1);
    record->display_type = row[2] ? atoi(row[2]) : 0;
    record->start_h = row[3] ? atoi(row[3]) : 0;
    record->end_h = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->occ_date, row[5], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_GetAll(DBConnectionManager* manager, DnfGameMessage* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date FROM dnf_game_message");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfGameMessage));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].message, row[1], sizeof(records[count].message) - 1);
        records[count].display_type = row[2] ? atoi(row[2]) : 0;
        records[count].start_h = row[3] ? atoi(row[3]) : 0;
        records[count].end_h = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].occ_date, row[5], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
