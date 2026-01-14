#include "log_playtime_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogPlaytime_Add(DBConnectionManager* manager, const LogPlaytime* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_playtime (m_id, charac_no, play_time, lobby_intime, lobby_outtime) "
        "VALUES (%d, %d, %d, '%s', '%s')",
        record->m_id, record->charac_no, record->play_time, record->lobby_intime, record->lobby_outtime);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogPlaytime_GetAll(DBConnectionManager* manager, LogPlaytime* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, play_time, lobby_intime, lobby_outtime FROM log_playtime");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPlaytime));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].play_time = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].lobby_intime, row[3], sizeof(records[count].lobby_intime) - 1);
        if (row[4]) strncpy(records[count].lobby_outtime, row[4], sizeof(records[count].lobby_outtime) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
