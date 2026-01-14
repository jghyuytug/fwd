#include "dnf_league_bbs_main_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfLeagueBbsMain_Add(DBConnectionManager* manager, const DnfLeagueBbsMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_league_bbs_main (league_id, main_cat, sub_cat, open_flag, title, reg_time, view) "
        "VALUES (%d, %d, %d, %d, '%s', %d, %d)",
        record->league_id, record->main_cat, record->sub_cat, record->open_flag, record->title, record->reg_time, record->view);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueBbsMain_Get(DBConnectionManager* manager, int bbs_id, DnfLeagueBbsMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_id, league_id, main_cat, sub_cat, open_flag, title, reg_time, view FROM dnf_league_bbs_main WHERE bbs_id = %d",
        bbs_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfLeagueBbsMain));
    record->bbs_id = row[0] ? atoi(row[0]) : 0;
    record->league_id = row[1] ? atoi(row[1]) : 0;
    record->main_cat = row[2] ? atoi(row[2]) : 0;
    record->sub_cat = row[3] ? atoi(row[3]) : 0;
    record->open_flag = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->title, row[5], sizeof(record->title) - 1);
    record->reg_time = row[6] ? atoi(row[6]) : 0;
    record->view = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueBbsMain_GetAll(DBConnectionManager* manager, DnfLeagueBbsMain* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_id, league_id, main_cat, sub_cat, open_flag, title, reg_time, view FROM dnf_league_bbs_main");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfLeagueBbsMain));
        records[count].bbs_id = row[0] ? atoi(row[0]) : 0;
        records[count].league_id = row[1] ? atoi(row[1]) : 0;
        records[count].main_cat = row[2] ? atoi(row[2]) : 0;
        records[count].sub_cat = row[3] ? atoi(row[3]) : 0;
        records[count].open_flag = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].title, row[5], sizeof(records[count].title) - 1);
        records[count].reg_time = row[6] ? atoi(row[6]) : 0;
        records[count].view = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
