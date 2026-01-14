#include "guild_stat_uv_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildStatUv_Add(DBConnectionManager* manager, const GuildStatUv* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_stat_uv (occ_date, pv, new_bbs, total_read_bbs, member_uv, member_uv_week, master_uv, master_uv_week) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->pv, record->new_bbs, record->total_read_bbs, record->member_uv, record->member_uv_week, record->master_uv, record->master_uv_week);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatUv_Get(DBConnectionManager* manager, const char* occ_date, GuildStatUv* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, pv, new_bbs, total_read_bbs, member_uv, member_uv_week, master_uv, master_uv_week FROM guild_stat_uv WHERE occ_date = '%s'",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildStatUv));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->pv = row[1] ? atoi(row[1]) : 0;
    record->new_bbs = row[2] ? atoi(row[2]) : 0;
    record->total_read_bbs = row[3] ? atoi(row[3]) : 0;
    record->member_uv = row[4] ? atoi(row[4]) : 0;
    record->member_uv_week = row[5] ? atoi(row[5]) : 0;
    record->master_uv = row[6] ? atoi(row[6]) : 0;
    record->master_uv_week = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStatUv_GetAll(DBConnectionManager* manager, GuildStatUv* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, pv, new_bbs, total_read_bbs, member_uv, member_uv_week, master_uv, master_uv_week FROM guild_stat_uv");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildStatUv));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].pv = row[1] ? atoi(row[1]) : 0;
        records[count].new_bbs = row[2] ? atoi(row[2]) : 0;
        records[count].total_read_bbs = row[3] ? atoi(row[3]) : 0;
        records[count].member_uv = row[4] ? atoi(row[4]) : 0;
        records[count].member_uv_week = row[5] ? atoi(row[5]) : 0;
        records[count].master_uv = row[6] ? atoi(row[6]) : 0;
        records[count].master_uv_week = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
