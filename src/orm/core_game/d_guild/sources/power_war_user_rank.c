#include "power_war_user_rank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CRUD Operations - Composite Primary Key (charac_no, server_id)

int PowerWarUserRank_Add(DBConnectionManager* manager, const PowerWarUserRank* record) {
    if (!manager || !record) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO power_war_user_rank (server_id, rank, charac_no, power_war_point, power_side) "
        "VALUES (%u, %u, %u, %u, %u)",
        record->server_id, record->rank, record->charac_no,
        record->power_war_point, record->power_side);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWarUserRank_Get(DBConnectionManager* manager, unsigned int charac_no, unsigned char server_id, PowerWarUserRank* out_record) {
    if (!manager || !out_record) return -1;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT server_id, rank, charac_no, power_war_point, power_side "
        "FROM power_war_user_rank WHERE charac_no = %u AND server_id = %u",
        charac_no, server_id);

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

    out_record->server_id = (unsigned char)atoi(row[0]);
    out_record->rank = (unsigned short)atoi(row[1]);
    out_record->charac_no = (unsigned int)atol(row[2]);
    out_record->power_war_point = (unsigned int)atol(row[3]);
    out_record->power_side = (unsigned char)atoi(row[4]);

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWarUserRank_Update(DBConnectionManager* manager, const PowerWarUserRank* record) {
    if (!manager || !record) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE power_war_user_rank SET rank = %u, power_war_point = %u, "
        "power_side = %u WHERE charac_no = %u AND server_id = %u",
        record->rank, record->power_war_point,
        record->power_side, record->charac_no, record->server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret > 0 ? 0 : -1;
}

int PowerWarUserRank_Delete(DBConnectionManager* manager, unsigned int charac_no, unsigned char server_id) {
    if (!manager) return -1;

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM power_war_user_rank WHERE charac_no = %u AND server_id = %u",
        charac_no, server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return ret >= 0 ? 0 : -1;
}

int PowerWarUserRank_Exists(DBConnectionManager* manager, unsigned int charac_no, unsigned char server_id) {
    if (!manager) return 0;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM power_war_user_rank WHERE charac_no = %u AND server_id = %u",
        charac_no, server_id);

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

int PowerWarUserRank_GetByServer(DBConnectionManager* manager, unsigned char server_id, PowerWarUserRank** out_records, int* out_count) {
    if (!manager || !out_records || !out_count) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT server_id, rank, charac_no, power_war_point, power_side "
        "FROM power_war_user_rank WHERE server_id = %u ORDER BY rank",
        server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int capacity = 16;
    PowerWarUserRank* records = (PowerWarUserRank*)malloc(sizeof(PowerWarUserRank) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWarUserRank* new_records = (PowerWarUserRank*)realloc(records, sizeof(PowerWarUserRank) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = (unsigned char)atoi(row[0]);
        records[count].rank = (unsigned short)atoi(row[1]);
        records[count].charac_no = (unsigned int)atol(row[2]);
        records[count].power_war_point = (unsigned int)atol(row[3]);
        records[count].power_side = (unsigned char)atoi(row[4]);
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}

int PowerWarUserRank_GetBySide(DBConnectionManager* manager, unsigned char power_side, PowerWarUserRank** out_records, int* out_count) {
    if (!manager || !out_records || !out_count) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT server_id, rank, charac_no, power_war_point, power_side "
        "FROM power_war_user_rank WHERE power_side = %u ORDER BY power_war_point DESC",
        power_side);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int capacity = 16;
    PowerWarUserRank* records = (PowerWarUserRank*)malloc(sizeof(PowerWarUserRank) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWarUserRank* new_records = (PowerWarUserRank*)realloc(records, sizeof(PowerWarUserRank) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = (unsigned char)atoi(row[0]);
        records[count].rank = (unsigned short)atoi(row[1]);
        records[count].charac_no = (unsigned int)atol(row[2]);
        records[count].power_war_point = (unsigned int)atol(row[3]);
        records[count].power_side = (unsigned char)atoi(row[4]);
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}

int PowerWarUserRank_GetTopRanks(DBConnectionManager* manager, unsigned char server_id, int limit, PowerWarUserRank** out_records, int* out_count) {
    if (!manager || !out_records || !out_count || limit <= 0) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT server_id, rank, charac_no, power_war_point, power_side "
        "FROM power_war_user_rank WHERE server_id = %u ORDER BY rank LIMIT %d",
        server_id, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int capacity = (limit > 16) ? limit : 16;
    PowerWarUserRank* records = (PowerWarUserRank*)malloc(sizeof(PowerWarUserRank) * capacity);
    if (!records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* row[5];
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (count >= capacity) {
            capacity *= 2;
            PowerWarUserRank* new_records = (PowerWarUserRank*)realloc(records, sizeof(PowerWarUserRank) * capacity);
            if (!new_records) {
                free(records);
                DBQueryResult_Free(&result);
                return -1;
            }
            records = new_records;
        }

        records[count].server_id = (unsigned char)atoi(row[0]);
        records[count].rank = (unsigned short)atoi(row[1]);
        records[count].charac_no = (unsigned int)atol(row[2]);
        records[count].power_war_point = (unsigned int)atol(row[3]);
        records[count].power_side = (unsigned char)atoi(row[4]);
        count++;
    }

    DBQueryResult_Free(&result);
    *out_records = records;
    *out_count = count;
    return 0;
}
