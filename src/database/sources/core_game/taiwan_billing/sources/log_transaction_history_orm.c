#include "log_transaction_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int LogTransactionHistory_Add(DBConnectionManager* manager, const LogTransactionHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_transaction_history (tran_id, tran_type, occ_date) "
        "VALUES (%llu, %u, '%s')",
        log->tran_id, log->tran_type, log->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogTransactionHistory_Get(DBConnectionManager* manager, unsigned long long tran_id,
                                LogTransactionHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_type, occ_date FROM log_transaction_history WHERE tran_id = %llu",
        tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(log, 0, sizeof(LogTransactionHistory));
    log->tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
    log->tran_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
    if (row[2]) strncpy(log->occ_date, row[2], sizeof(log->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogTransactionHistory_Update(DBConnectionManager* manager, const LogTransactionHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_transaction_history SET tran_type = %u, occ_date = '%s' WHERE tran_id = %llu",
        log->tran_type, log->occ_date, log->tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogTransactionHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_transaction_history WHERE tran_id = %llu", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogTransactionHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM log_transaction_history WHERE tran_id = %llu LIMIT 1", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int LogTransactionHistory_GetAll(DBConnectionManager* manager, LogTransactionHistory* logs,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_type, occ_date FROM log_transaction_history ORDER BY occ_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogTransactionHistory));
        logs[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        logs[count].tran_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].occ_date, row[2], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogTransactionHistory_GetByType(DBConnectionManager* manager, unsigned char tran_type,
                                     LogTransactionHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_type, occ_date FROM log_transaction_history "
        "WHERE tran_type = %u ORDER BY occ_date DESC", tran_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogTransactionHistory));
        logs[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        logs[count].tran_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].occ_date, row[2], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogTransactionHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                           const char* end_date, LogTransactionHistory* logs,
                                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !start_date || !end_date || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_type, occ_date FROM log_transaction_history "
        "WHERE occ_date BETWEEN '%s' AND '%s' ORDER BY occ_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogTransactionHistory));
        logs[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        logs[count].tran_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].occ_date, row[2], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void LogTransactionHistory_PrintInfo(const LogTransactionHistory* log) {
    if (!log) return;

    printf("=== Log Transaction History Info ===\n");
    printf("Transaction ID: %llu\n", log->tran_id);
    printf("Transaction Type: %u\n", log->tran_type);
    printf("Occurred Date: %s\n", log->occ_date);
    printf("===================================\n");
}
