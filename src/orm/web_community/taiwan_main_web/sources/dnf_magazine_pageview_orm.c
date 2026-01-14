#include "dnf_magazine_pageview_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfMagazinePageview_Add(DBConnectionManager* manager, const DnfMagazinePageview* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_magazine_pageview (magazine_id, pageview) "
        "VALUES (%d, %d)",
        record->magazine_id, record->pageview);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMagazinePageview_Get(DBConnectionManager* manager, int magazine_id, DnfMagazinePageview* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT magazine_id, pageview FROM dnf_magazine_pageview WHERE magazine_id = %d",
        magazine_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfMagazinePageview));
    record->magazine_id = row[0] ? atoi(row[0]) : 0;
    record->pageview = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMagazinePageview_GetAll(DBConnectionManager* manager, DnfMagazinePageview* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT magazine_id, pageview FROM dnf_magazine_pageview");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfMagazinePageview));
        records[count].magazine_id = row[0] ? atoi(row[0]) : 0;
        records[count].pageview = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
