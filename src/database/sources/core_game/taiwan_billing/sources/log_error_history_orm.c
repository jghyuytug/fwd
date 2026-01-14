#include "log_error_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations */

int LogErrorHistory_Add(DBConnectionManager* manager, const LogErrorHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    /* no is AUTO_INCREMENT, not specified in INSERT */
    if (record->occ_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO log_error_history (error_id, error_msg, error_query, "
            "proc_name, proc_line, query_user, occ_date) "
            "VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s')",
            record->error_id, record->error_msg, record->error_query,
            record->proc_name, record->proc_line, record->query_user, record->occ_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO log_error_history (error_id, error_msg, error_query, "
            "proc_name, proc_line, query_user, occ_date) "
            "VALUES (%d, '%s', '%s', '%s', %d, '%s', NOW())",
            record->error_id, record->error_msg, record->error_query,
            record->proc_name, record->proc_line, record->query_user);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_Get(DBConnectionManager* manager, unsigned int no, LogErrorHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, error_id, error_msg, error_query, proc_name, proc_line, "
        "query_user, occ_date FROM log_error_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogErrorHistory));
    record->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    record->error_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->error_msg, row[2], sizeof(record->error_msg) - 1);
    if (row[3]) strncpy(record->error_query, row[3], sizeof(record->error_query) - 1);
    if (row[4]) strncpy(record->proc_name, row[4], sizeof(record->proc_name) - 1);
    record->proc_line = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->query_user, row[6], sizeof(record->query_user) - 1);
    if (row[7]) strncpy(record->occ_date, row[7], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_error_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_error_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Business Operations */

int LogErrorHistory_GetRecent(DBConnectionManager* manager, LogErrorHistory* records,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, error_id, error_msg, error_query, proc_name, proc_line, "
        "query_user, occ_date FROM log_error_history "
        "ORDER BY no DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogErrorHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].error_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].error_msg, row[2], sizeof(records[count].error_msg) - 1);
        if (row[3]) strncpy(records[count].error_query, row[3], sizeof(records[count].error_query) - 1);
        if (row[4]) strncpy(records[count].proc_name, row[4], sizeof(records[count].proc_name) - 1);
        records[count].proc_line = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].query_user, row[6], sizeof(records[count].query_user) - 1);
        if (row[7]) strncpy(records[count].occ_date, row[7], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_GetByErrorId(DBConnectionManager* manager, int error_id,
                                  LogErrorHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, error_id, error_msg, error_query, proc_name, proc_line, "
        "query_user, occ_date FROM log_error_history "
        "WHERE error_id = %d ORDER BY no DESC LIMIT %d", error_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogErrorHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].error_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].error_msg, row[2], sizeof(records[count].error_msg) - 1);
        if (row[3]) strncpy(records[count].error_query, row[3], sizeof(records[count].error_query) - 1);
        if (row[4]) strncpy(records[count].proc_name, row[4], sizeof(records[count].proc_name) - 1);
        records[count].proc_line = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].query_user, row[6], sizeof(records[count].query_user) - 1);
        if (row[7]) strncpy(records[count].occ_date, row[7], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_GetByProcName(DBConnectionManager* manager, const char* proc_name,
                                   LogErrorHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !proc_name || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, error_id, error_msg, error_query, proc_name, proc_line, "
        "query_user, occ_date FROM log_error_history "
        "WHERE proc_name = '%s' ORDER BY no DESC LIMIT %d", proc_name, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogErrorHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].error_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].error_msg, row[2], sizeof(records[count].error_msg) - 1);
        if (row[3]) strncpy(records[count].error_query, row[3], sizeof(records[count].error_query) - 1);
        if (row[4]) strncpy(records[count].proc_name, row[4], sizeof(records[count].proc_name) - 1);
        records[count].proc_line = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].query_user, row[6], sizeof(records[count].query_user) - 1);
        if (row[7]) strncpy(records[count].occ_date, row[7], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_GetByDateRange(DBConnectionManager* manager,
                                    const char* start_date, const char* end_date,
                                    LogErrorHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !start_date || !end_date || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, error_id, error_msg, error_query, proc_name, proc_line, "
        "query_user, occ_date FROM log_error_history "
        "WHERE occ_date BETWEEN '%s' AND '%s' ORDER BY no DESC LIMIT %d",
        start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogErrorHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].error_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].error_msg, row[2], sizeof(records[count].error_msg) - 1);
        if (row[3]) strncpy(records[count].error_query, row[3], sizeof(records[count].error_query) - 1);
        if (row[4]) strncpy(records[count].proc_name, row[4], sizeof(records[count].proc_name) - 1);
        records[count].proc_line = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].query_user, row[6], sizeof(records[count].query_user) - 1);
        if (row[7]) strncpy(records[count].occ_date, row[7], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_DeleteOlderThan(DBConnectionManager* manager, const char* date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_error_history WHERE occ_date < '%s'", date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogErrorHistory_GetCount(DBConnectionManager* manager, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_error_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *count = row[0] ? atoi(row[0]) : 0;
    } else {
        *count = 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Utility Functions */

void LogErrorHistory_PrintInfo(const LogErrorHistory* record) {
    if (!record) return;

    printf("No: %u\n", record->no);
    printf("Error ID: %d\n", record->error_id);
    printf("Error Msg: %s\n", record->error_msg);
    printf("Error Query: %s\n", record->error_query);
    printf("Proc Name: %s\n", record->proc_name);
    printf("Proc Line: %d\n", record->proc_line);
    printf("Query User: %s\n", record->query_user);
    printf("Occ Date: %s\n", record->occ_date);
}
