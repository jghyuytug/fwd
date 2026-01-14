#include "power_war_statue_ranker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CRUD Operations

int PowerWarStatueRanker_Add(DBConnectionManager* manager, const PowerWarStatueRanker* record) {
    if (!manager || !record) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO power_war_statue_ranker (server_id, first_ranker, second_ranker, third_ranker) "
        "VALUES (%u, %u, %u, %u)",
        record->server_id, record->first_ranker,
        record->second_ranker, record->third_ranker);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWarStatueRanker_Get(DBConnectionManager* manager, unsigned char server_id, PowerWarStatueRanker* out_record) {
    if (!manager || !out_record) return -1;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT server_id, first_ranker, second_ranker, third_ranker "
        "FROM power_war_statue_ranker WHERE server_id = %u",
        server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[4];
    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    out_record->server_id = (unsigned char)atoi(row[0]);
    out_record->first_ranker = (unsigned int)atol(row[1]);
    out_record->second_ranker = (unsigned int)atol(row[2]);
    out_record->third_ranker = (unsigned int)atol(row[3]);

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWarStatueRanker_Update(DBConnectionManager* manager, const PowerWarStatueRanker* record) {
    if (!manager || !record) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE power_war_statue_ranker SET first_ranker = %u, second_ranker = %u, "
        "third_ranker = %u WHERE server_id = %u",
        record->first_ranker, record->second_ranker,
        record->third_ranker, record->server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWarStatueRanker_Delete(DBConnectionManager* manager, unsigned char server_id) {
    if (!manager) return -1;

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM power_war_statue_ranker WHERE server_id = %u", server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret >= 0 ? 0 : -1;
}

int PowerWarStatueRanker_Exists(DBConnectionManager* manager, unsigned char server_id) {
    if (!manager) return 0;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM power_war_statue_ranker WHERE server_id = %u", server_id);

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

int PowerWarStatueRanker_GetAllServers(DBConnectionManager* manager, PowerWarStatueRanker** out_records, int* out_count) {
    if (!manager || !out_records || !out_count) return -1;

    const char* query = "SELECT server_id, first_ranker, second_ranker, third_ranker "
                       "FROM power_war_statue_ranker ORDER BY server_id";

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int capacity = 16;
    PowerWarStatueRanker* records = (PowerWarStatueRanker*)malloc(sizeof(PowerWarStatueRanker) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[4];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWarStatueRanker* new_records = (PowerWarStatueRanker*)realloc(records, sizeof(PowerWarStatueRanker) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = (unsigned char)atoi(row[0]);
        records[count].first_ranker = (unsigned int)atol(row[1]);
        records[count].second_ranker = (unsigned int)atol(row[2]);
        records[count].third_ranker = (unsigned int)atol(row[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}

int PowerWarStatueRanker_UpdateRanker(DBConnectionManager* manager, unsigned char server_id, int position, unsigned int charac_no) {
    if (!manager || position < 1 || position > 3) return -1;

    const char* field_names[] = {"first_ranker", "second_ranker", "third_ranker"};
    const char* field = field_names[position - 1];

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE power_war_statue_ranker SET %s = %u WHERE server_id = %u",
        field, charac_no, server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWarStatueRanker_GetRankerPosition(DBConnectionManager* manager, unsigned int charac_no, unsigned char* out_server_id, int* out_position) {
    if (!manager || !out_server_id || !out_position) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT server_id, "
        "CASE "
        "  WHEN first_ranker = %u THEN 1 "
        "  WHEN second_ranker = %u THEN 2 "
        "  WHEN third_ranker = %u THEN 3 "
        "  ELSE 0 "
        "END AS position "
        "FROM power_war_statue_ranker "
        "WHERE first_ranker = %u OR second_ranker = %u OR third_ranker = %u",
        charac_no, charac_no, charac_no, charac_no, charac_no, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[2];
    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *out_server_id = (unsigned char)atoi(row[0]);
    *out_position = atoi(row[1]);

    DBQueryResult_Free(&result);
    return 0;
}
