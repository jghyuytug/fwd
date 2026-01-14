#include "ucc_user_writers_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UccUserWriters_Add(DBConnectionManager* manager, const UccUserWriters* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ucc_user_writers (m_id, server_id, charac_no, charac_name, start_time, end_time, comment) "
        "VALUES (%d, %d, %d, '%s', %d, %d, '%s')",
        record->m_id, record->server_id, record->charac_no, record->charac_name, record->start_time, record->end_time, record->comment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UccUserWriters_GetAll(DBConnectionManager* manager, UccUserWriters* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, charac_name, start_time, end_time, comment FROM ucc_user_writers");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UccUserWriters));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
        records[count].start_time = row[4] ? atoi(row[4]) : 0;
        records[count].end_time = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].comment, row[6], sizeof(records[count].comment) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
