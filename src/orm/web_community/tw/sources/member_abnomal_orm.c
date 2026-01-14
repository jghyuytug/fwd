#include "member_abnomal_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberAbnomal_Add(DBConnectionManager* manager, const MemberAbnomal* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_abnomal (user_id, overlab_count) "
        "VALUES ('%s', %d)",
        record->user_id, record->overlab_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberAbnomal_Get(DBConnectionManager* manager, const char* user_id, MemberAbnomal* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count FROM member_abnomal WHERE user_id = '%s'",
        user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberAbnomal));
    if (row[0]) strncpy(record->user_id, row[0], sizeof(record->user_id) - 1);
    record->overlab_count = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberAbnomal_GetAll(DBConnectionManager* manager, MemberAbnomal* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count FROM member_abnomal");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberAbnomal));
        if (row[0]) strncpy(records[count].user_id, row[0], sizeof(records[count].user_id) - 1);
        records[count].overlab_count = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
