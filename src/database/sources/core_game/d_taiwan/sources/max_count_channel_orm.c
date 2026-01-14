#include "max_count_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MaxCountChannel_Add(DBConnectionManager* manager, const MaxCountChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO max_count_channel (server_info, gc_channeltype, mc_max, mc_date) "
        "VALUES (%u, '%s', %u, '%s')",
        record->server_info, record->gc_channeltype,
        record->mc_max, record->mc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_Get(DBConnectionManager* manager, unsigned char server_info,
                        const char* gc_channeltype, MaxCountChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !gc_channeltype || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date "
        "FROM max_count_channel WHERE server_info = %u AND gc_channeltype = '%s'",
        server_info, gc_channeltype);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MaxCountChannel));
    record->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    if (row[1]) strncpy(record->gc_channeltype, row[1], sizeof(record->gc_channeltype) - 1);
    record->mc_max = row[2] ? (unsigned int)atoi(row[2]) : 0;
    if (row[3]) strncpy(record->mc_date, row[3], sizeof(record->mc_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_Update(DBConnectionManager* manager, const MaxCountChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE max_count_channel SET mc_max = %u, mc_date = '%s' "
        "WHERE server_info = %u AND gc_channeltype = '%s'",
        record->mc_max, record->mc_date,
        record->server_info, record->gc_channeltype);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_Delete(DBConnectionManager* manager, unsigned char server_info,
                           const char* gc_channeltype) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !gc_channeltype) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM max_count_channel WHERE server_info = %u AND gc_channeltype = '%s'",
        server_info, gc_channeltype);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_Exists(DBConnectionManager* manager, unsigned char server_info,
                           const char* gc_channeltype) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !gc_channeltype) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info FROM max_count_channel "
        "WHERE server_info = %u AND gc_channeltype = '%s'",
        server_info, gc_channeltype);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int MaxCountChannel_GetAll(DBConnectionManager* manager, MaxCountChannel* records,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date "
        "FROM max_count_channel ORDER BY server_info, gc_channeltype");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountChannel));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].gc_channeltype, row[1], sizeof(records[count].gc_channeltype) - 1);
        records[count].mc_max = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].mc_date, row[3], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_GetByServer(DBConnectionManager* manager, unsigned char server_info,
                                 MaxCountChannel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date "
        "FROM max_count_channel WHERE server_info = %u ORDER BY gc_channeltype",
        server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountChannel));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].gc_channeltype, row[1], sizeof(records[count].gc_channeltype) - 1);
        records[count].mc_max = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].mc_date, row[3], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_GetByChannelType(DBConnectionManager* manager, const char* gc_channeltype,
                                      MaxCountChannel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !gc_channeltype || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date "
        "FROM max_count_channel WHERE gc_channeltype = '%s' ORDER BY server_info",
        gc_channeltype);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountChannel));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].gc_channeltype, row[1], sizeof(records[count].gc_channeltype) - 1);
        records[count].mc_max = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].mc_date, row[3], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, MaxCountChannel* records,
                                    int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !start_date || !end_date || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date "
        "FROM max_count_channel WHERE mc_date >= '%s' AND mc_date <= '%s' "
        "ORDER BY mc_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountChannel));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].gc_channeltype, row[1], sizeof(records[count].gc_channeltype) - 1);
        records[count].mc_max = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].mc_date, row[3], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountChannel_GetMaxRecord(DBConnectionManager* manager, unsigned char server_info,
                                  MaxCountChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, gc_channeltype, mc_max, mc_date "
        "FROM max_count_channel WHERE server_info = %u ORDER BY mc_max DESC LIMIT 1",
        server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MaxCountChannel));
    record->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    if (row[1]) strncpy(record->gc_channeltype, row[1], sizeof(record->gc_channeltype) - 1);
    record->mc_max = row[2] ? (unsigned int)atoi(row[2]) : 0;
    if (row[3]) strncpy(record->mc_date, row[3], sizeof(record->mc_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

void MaxCountChannel_PrintInfo(const MaxCountChannel* record) {
    if (!record) return;

    printf("Max Count Channel Info:\n");
    printf("  Server Info: %u\n", record->server_info);
    printf("  Channel Type: %s\n", record->gc_channeltype);
    printf("  Max Count: %u\n", record->mc_max);
    printf("  Date: %s\n", record->mc_date);
}
