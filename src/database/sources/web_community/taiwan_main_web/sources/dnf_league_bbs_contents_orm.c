#include "dnf_league_bbs_contents_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfLeagueBbsContents_Add(DBConnectionManager* manager, const DnfLeagueBbsContents* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_league_bbs_contents (bbs_id, m_id, broadcast_date, thumbnail_img, attach_file, contents) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s')",
        record->bbs_id, record->m_id, record->broadcast_date, record->thumbnail_img, record->attach_file, record->contents);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueBbsContents_Get(DBConnectionManager* manager, int bbs_id, DnfLeagueBbsContents* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_id, m_id, broadcast_date, thumbnail_img, attach_file, contents FROM dnf_league_bbs_contents WHERE bbs_id = %d",
        bbs_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfLeagueBbsContents));
    record->bbs_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->broadcast_date = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->thumbnail_img, row[3], sizeof(record->thumbnail_img) - 1);
    if (row[4]) strncpy(record->attach_file, row[4], sizeof(record->attach_file) - 1);
    if (row[5]) strncpy(record->contents, row[5], sizeof(record->contents) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueBbsContents_GetAll(DBConnectionManager* manager, DnfLeagueBbsContents* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_id, m_id, broadcast_date, thumbnail_img, attach_file, contents FROM dnf_league_bbs_contents");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfLeagueBbsContents));
        records[count].bbs_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].broadcast_date = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].thumbnail_img, row[3], sizeof(records[count].thumbnail_img) - 1);
        if (row[4]) strncpy(records[count].attach_file, row[4], sizeof(records[count].attach_file) - 1);
        if (row[5]) strncpy(records[count].contents, row[5], sizeof(records[count].contents) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
