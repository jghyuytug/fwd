#include "event_quizquiz_stamp_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventQuizquizStamp_Add(DBConnectionManager* manager, const EventQuizquizStamp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->occ_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO event_quizquiz_stamp (m_id, degree, stamp, occ_time) "
            "VALUES (%d, %d, %d, '%s')",
            record->m_id, record->degree, record->stamp, record->occ_time);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO event_quizquiz_stamp (m_id, degree, stamp, occ_time) "
            "VALUES (%d, %d, %d, NOW())",
            record->m_id, record->degree, record->stamp);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_Get(DBConnectionManager* manager, int m_id, char degree,
                            EventQuizquizStamp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, degree, stamp, occ_time FROM event_quizquiz_stamp "
        "WHERE m_id = %d AND degree = %d",
        m_id, degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventQuizquizStamp));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->degree = row[1] ? (char)atoi(row[1]) : 0;
    record->stamp = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_time, row[3], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_Update(DBConnectionManager* manager, const EventQuizquizStamp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_quizquiz_stamp SET stamp = %d, occ_time = '%s' "
        "WHERE m_id = %d AND degree = %d",
        record->stamp, record->occ_time, record->m_id, record->degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_Delete(DBConnectionManager* manager, int m_id, char degree) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_quizquiz_stamp WHERE m_id = %d AND degree = %d",
        m_id, degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_Exists(DBConnectionManager* manager, int m_id, char degree) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM event_quizquiz_stamp WHERE m_id = %d AND degree = %d LIMIT 1",
        m_id, degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventQuizquizStamp_GetByMemberId(DBConnectionManager* manager, int m_id,
                                      EventQuizquizStamp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, degree, stamp, occ_time FROM event_quizquiz_stamp "
        "WHERE m_id = %d ORDER BY degree",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventQuizquizStamp));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].degree = row[1] ? (char)atoi(row[1]) : 0;
        records[count].stamp = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_GetByDegree(DBConnectionManager* manager, char degree,
                                    EventQuizquizStamp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, degree, stamp, occ_time FROM event_quizquiz_stamp "
        "WHERE degree = %d ORDER BY m_id",
        degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventQuizquizStamp));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].degree = row[1] ? (char)atoi(row[1]) : 0;
        records[count].stamp = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_GetAll(DBConnectionManager* manager, EventQuizquizStamp* records,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, degree, stamp, occ_time FROM event_quizquiz_stamp "
        "ORDER BY m_id, degree");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventQuizquizStamp));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].degree = row[1] ? (char)atoi(row[1]) : 0;
        records[count].stamp = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventQuizquizStamp_UpdateStamp(DBConnectionManager* manager, int m_id, char degree,
                                    int stamp_increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_quizquiz_stamp SET stamp = stamp + %d, occ_time = NOW() "
        "WHERE m_id = %d AND degree = %d",
        stamp_increment, m_id, degree);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventQuizquizStamp_PrintInfo(const EventQuizquizStamp* record) {
    if (!record) return;

    printf("EventQuizquizStamp:\n");
    printf("  m_id: %d\n", record->m_id);
    printf("  degree: %d\n", record->degree);
    printf("  stamp: %d\n", record->stamp);
    printf("  occ_time: %s\n", record->occ_time);
}
