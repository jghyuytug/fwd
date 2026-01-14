#include "dnf_league_player_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfLeaguePlayerInfo_Add(DBConnectionManager* manager, const DnfLeaguePlayerInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_league_player_info (league_id, sub_cat, bbs_id, title, m_id, name, server_id, charac_no, charac_name, job, grow_type, picture) "
        "VALUES (%d, %d, %d, '%s', %d, '%s', %d, %d, '%s', %d, %d, '%s')",
        record->league_id, record->sub_cat, record->bbs_id, record->title, record->m_id, record->name, record->server_id, record->charac_no, record->charac_name, record->job, record->grow_type, record->picture);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeaguePlayerInfo_Get(DBConnectionManager* manager, int pi_id, DnfLeaguePlayerInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT pi_id, league_id, sub_cat, bbs_id, title, m_id, name, server_id, charac_no, charac_name, job, grow_type, picture FROM dnf_league_player_info WHERE pi_id = %d",
        pi_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfLeaguePlayerInfo));
    record->pi_id = row[0] ? atoi(row[0]) : 0;
    record->league_id = row[1] ? atoi(row[1]) : 0;
    record->sub_cat = row[2] ? atoi(row[2]) : 0;
    record->bbs_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->title, row[4], sizeof(record->title) - 1);
    record->m_id = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->name, row[6], sizeof(record->name) - 1);
    record->server_id = row[7] ? atoi(row[7]) : 0;
    record->charac_no = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->charac_name, row[9], sizeof(record->charac_name) - 1);
    record->job = row[10] ? atoi(row[10]) : 0;
    record->grow_type = row[11] ? atoi(row[11]) : 0;
    if (row[12]) strncpy(record->picture, row[12], sizeof(record->picture) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeaguePlayerInfo_GetAll(DBConnectionManager* manager, DnfLeaguePlayerInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pi_id, league_id, sub_cat, bbs_id, title, m_id, name, server_id, charac_no, charac_name, job, grow_type, picture FROM dnf_league_player_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfLeaguePlayerInfo));
        records[count].pi_id = row[0] ? atoi(row[0]) : 0;
        records[count].league_id = row[1] ? atoi(row[1]) : 0;
        records[count].sub_cat = row[2] ? atoi(row[2]) : 0;
        records[count].bbs_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].title, row[4], sizeof(records[count].title) - 1);
        records[count].m_id = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].name, row[6], sizeof(records[count].name) - 1);
        records[count].server_id = row[7] ? atoi(row[7]) : 0;
        records[count].charac_no = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].charac_name, row[9], sizeof(records[count].charac_name) - 1);
        records[count].job = row[10] ? atoi(row[10]) : 0;
        records[count].grow_type = row[11] ? atoi(row[11]) : 0;
        if (row[12]) strncpy(records[count].picture, row[12], sizeof(records[count].picture) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
