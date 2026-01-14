#include "wiki_sync_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WikiSync_Add(DBConnectionManager* manager, const WikiSync* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO wiki_sync (gno, b2t_time, t2r_time, r2s_time, b2t_admin, t2r_admin, r2s_admin, state) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
        record->gno, record->b2t_time, record->t2r_time, record->r2s_time, record->b2t_admin, record->t2r_admin, record->r2s_admin, record->state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WikiSync_GetAll(DBConnectionManager* manager, WikiSync* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, b2t_time, t2r_time, r2s_time, b2t_admin, t2r_admin, r2s_admin, state FROM wiki_sync");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WikiSync));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        records[count].b2t_time = row[1] ? atoi(row[1]) : 0;
        records[count].t2r_time = row[2] ? atoi(row[2]) : 0;
        records[count].r2s_time = row[3] ? atoi(row[3]) : 0;
        records[count].b2t_admin = row[4] ? atoi(row[4]) : 0;
        records[count].t2r_admin = row[5] ? atoi(row[5]) : 0;
        records[count].r2s_admin = row[6] ? atoi(row[6]) : 0;
        records[count].state = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
