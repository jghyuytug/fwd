#include "dnf_ad_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfAdHistory_Add(DBConnectionManager* manager, const DnfAdHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_ad_history (ad_id, occ_time, open, click, login, logout) "
        "VALUES (%d, %d, %d, %d, %d, %d)",
        record->ad_id, record->occ_time, record->open, record->click, record->login, record->logout);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfAdHistory_Get(DBConnectionManager* manager, short ad_id, int occ_time, DnfAdHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ad_id, occ_time, open, click, login, logout FROM dnf_ad_history WHERE ad_id = %d AND occ_time = %d",
        ad_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfAdHistory));
    record->ad_id = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->open = row[2] ? atoi(row[2]) : 0;
    record->click = row[3] ? atoi(row[3]) : 0;
    record->login = row[4] ? atoi(row[4]) : 0;
    record->logout = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfAdHistory_GetAll(DBConnectionManager* manager, DnfAdHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ad_id, occ_time, open, click, login, logout FROM dnf_ad_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfAdHistory));
        records[count].ad_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].open = row[2] ? atoi(row[2]) : 0;
        records[count].click = row[3] ? atoi(row[3]) : 0;
        records[count].login = row[4] ? atoi(row[4]) : 0;
        records[count].logout = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
