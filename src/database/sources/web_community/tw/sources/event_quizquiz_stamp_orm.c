#include "event_quizquiz_stamp_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventQuizquizStamp_Add(DBConnectionManager* manager, const EventQuizquizStamp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_quizquiz_stamp (m_id, degree, stamp, occ_time) "
        "VALUES (%d, %d, %d, '%s')",
        record->m_id, record->degree, record->stamp, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_Get(DBConnectionManager* manager, int m_id, signed char degree, EventQuizquizStamp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, degree, stamp, occ_time FROM event_quizquiz_stamp WHERE m_id = %d AND degree = %d",
        m_id, degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventQuizquizStamp));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->degree = row[1] ? atoi(row[1]) : 0;
    record->stamp = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_time, row[3], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_GetAll(DBConnectionManager* manager, EventQuizquizStamp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, degree, stamp, occ_time FROM event_quizquiz_stamp");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventQuizquizStamp));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].degree = row[1] ? atoi(row[1]) : 0;
        records[count].stamp = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
