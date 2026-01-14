#include "member_id_pass_change_pv_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberIdPassChangePv_Add(DBConnectionManager* manager, const MemberIdPassChangePv* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_id_pass_change_pv (occ_date, type, count) "
        "VALUES ('%s', %d, %d)",
        record->occ_date, record->type, record->count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberIdPassChangePv_Get(DBConnectionManager* manager, int id, MemberIdPassChangePv* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, occ_date, type, count FROM member_id_pass_change_pv WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberIdPassChangePv));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->type = row[2] ? atoi(row[2]) : 0;
    record->count = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberIdPassChangePv_GetAll(DBConnectionManager* manager, MemberIdPassChangePv* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, occ_date, type, count FROM member_id_pass_change_pv");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberIdPassChangePv));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].type = row[2] ? atoi(row[2]) : 0;
        records[count].count = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
