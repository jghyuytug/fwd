#include "dnf_league_mainpage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfLeagueMainpage_Add(DBConnectionManager* manager, const DnfLeagueMainpage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_league_mainpage (top_img, broadcast_time_img, vod_link, contents, notice) "
        "VALUES ('%s', '%s', '%s', '%s', '%s')",
        record->top_img, record->broadcast_time_img, record->vod_link, record->contents, record->notice);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueMainpage_Get(DBConnectionManager* manager, int mp_id, DnfLeagueMainpage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT mp_id, top_img, broadcast_time_img, vod_link, contents, notice FROM dnf_league_mainpage WHERE mp_id = %d",
        mp_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfLeagueMainpage));
    record->mp_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->top_img, row[1], sizeof(record->top_img) - 1);
    if (row[2]) strncpy(record->broadcast_time_img, row[2], sizeof(record->broadcast_time_img) - 1);
    if (row[3]) strncpy(record->vod_link, row[3], sizeof(record->vod_link) - 1);
    if (row[4]) strncpy(record->contents, row[4], sizeof(record->contents) - 1);
    if (row[5]) strncpy(record->notice, row[5], sizeof(record->notice) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueMainpage_GetAll(DBConnectionManager* manager, DnfLeagueMainpage* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT mp_id, top_img, broadcast_time_img, vod_link, contents, notice FROM dnf_league_mainpage");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfLeagueMainpage));
        records[count].mp_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].top_img, row[1], sizeof(records[count].top_img) - 1);
        if (row[2]) strncpy(records[count].broadcast_time_img, row[2], sizeof(records[count].broadcast_time_img) - 1);
        if (row[3]) strncpy(records[count].vod_link, row[3], sizeof(records[count].vod_link) - 1);
        if (row[4]) strncpy(records[count].contents, row[4], sizeof(records[count].contents) - 1);
        if (row[5]) strncpy(records[count].notice, row[5], sizeof(records[count].notice) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
