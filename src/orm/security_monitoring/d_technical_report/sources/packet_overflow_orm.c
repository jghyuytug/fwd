#include "packet_overflow_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PacketOverflow_Add(DBConnectionManager* manager, const PacketOverflow* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO packet_overflow (packet_type, packet_kind, cnt) "
        "VALUES (%d, '%s', %d)",
        record->packet_type, record->packet_kind, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PacketOverflow_Get(DBConnectionManager* manager, short packet_type, const char* packet_kind, PacketOverflow* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT packet_type, packet_kind, cnt FROM packet_overflow WHERE packet_type = %d AND packet_kind = '%s'",
        packet_type, packet_kind);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PacketOverflow));
    record->packet_type = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->packet_kind, row[1], sizeof(record->packet_kind) - 1);
    record->cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PacketOverflow_GetAll(DBConnectionManager* manager, PacketOverflow* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT packet_type, packet_kind, cnt FROM packet_overflow");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PacketOverflow));
        records[count].packet_type = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].packet_kind, row[1], sizeof(records[count].packet_kind) - 1);
        records[count].cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
