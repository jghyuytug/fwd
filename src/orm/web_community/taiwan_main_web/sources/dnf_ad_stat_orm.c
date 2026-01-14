#include "dnf_ad_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfAdStat_Add(DBConnectionManager* manager, const DnfAdStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_ad_stat (ad_id, open, click, login, logout) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->ad_id, record->open, record->click, record->login, record->logout);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfAdStat_Get(DBConnectionManager* manager, short ad_id, DnfAdStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ad_id, open, click, login, logout FROM dnf_ad_stat WHERE ad_id = %d",
        ad_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfAdStat));
    record->ad_id = row[0] ? atoi(row[0]) : 0;
    record->open = row[1] ? atoi(row[1]) : 0;
    record->click = row[2] ? atoi(row[2]) : 0;
    record->login = row[3] ? atoi(row[3]) : 0;
    record->logout = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfAdStat_GetAll(DBConnectionManager* manager, DnfAdStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ad_id, open, click, login, logout FROM dnf_ad_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfAdStat));
        records[count].ad_id = row[0] ? atoi(row[0]) : 0;
        records[count].open = row[1] ? atoi(row[1]) : 0;
        records[count].click = row[2] ? atoi(row[2]) : 0;
        records[count].login = row[3] ? atoi(row[3]) : 0;
        records[count].logout = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
