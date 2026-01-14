#include "guild_stat_month_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildStatMonth_Add(DBConnectionManager* manager, const GuildStatMonth* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_stat_month (occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_date, record->lev, record->server_id, record->avg_guild_point, record->avg_guild_point_acc);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_Get(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id, GuildStatMonth* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc FROM guild_stat_month WHERE occ_date = '%s' AND lev = %d AND server_id = %d",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildStatMonth));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->lev = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->avg_guild_point = row[3] ? atoi(row[3]) : 0;
    record->avg_guild_point_acc = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatMonth_GetAll(DBConnectionManager* manager, GuildStatMonth* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, avg_guild_point, avg_guild_point_acc FROM guild_stat_month");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildStatMonth));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].lev = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].avg_guild_point = row[3] ? atoi(row[3]) : 0;
        records[count].avg_guild_point_acc = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
