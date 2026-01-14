#include "bad_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BadUser_Add(DBConnectionManager* manager, const BadUser* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO bad_user (m_id, bad_code, create_day, exit_day, admin_n) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->m_id, record->bad_code, record->create_day, record->exit_day, record->admin_n);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BadUser_Get(DBConnectionManager* manager, int no, BadUser* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n FROM bad_user WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(BadUser));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->bad_code = row[2] ? atoi(row[2]) : 0;
    record->create_day = row[3] ? atoi(row[3]) : 0;
    record->exit_day = row[4] ? atoi(row[4]) : 0;
    record->admin_n = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int BadUser_GetAll(DBConnectionManager* manager, BadUser* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n FROM bad_user");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BadUser));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].bad_code = row[2] ? atoi(row[2]) : 0;
        records[count].create_day = row[3] ? atoi(row[3]) : 0;
        records[count].exit_day = row[4] ? atoi(row[4]) : 0;
        records[count].admin_n = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
