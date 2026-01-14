#include "log_refund_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogRefundHistory_Add(DBConnectionManager* manager, const LogRefundHistory* refund) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !refund) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_refund_history (tran_id, account_id, order_tran_id, "
        "amount, tran_state, query_user, occ_date) "
        "VALUES (%llu, '%s', '%s', %u, %u, '%s', '%s')",
        refund->tran_id, refund->account_id, refund->order_tran_id,
        refund->amount, refund->tran_state, refund->query_user, refund->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_Get(DBConnectionManager* manager, unsigned long long tran_id,
                          const char* account_id, LogRefundHistory* refund) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !account_id || !refund) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, account_id, order_tran_id, amount, tran_state, "
        "query_user, occ_date FROM log_refund_history "
        "WHERE tran_id = %llu AND account_id = '%s'",
        tran_id, account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(refund, 0, sizeof(LogRefundHistory));
    refund->tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
    if (row[1]) strncpy(refund->account_id, row[1], sizeof(refund->account_id) - 1);
    if (row[2]) strncpy(refund->order_tran_id, row[2], sizeof(refund->order_tran_id) - 1);
    refund->amount = row[3] ? (unsigned int)atoi(row[3]) : 0;
    refund->tran_state = row[4] ? (unsigned char)atoi(row[4]) : 0;
    if (row[5]) strncpy(refund->query_user, row[5], sizeof(refund->query_user) - 1);
    if (row[6]) strncpy(refund->occ_date, row[6], sizeof(refund->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_Update(DBConnectionManager* manager, const LogRefundHistory* refund) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !refund) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_refund_history SET order_tran_id = '%s', amount = %u, "
        "tran_state = %u, query_user = '%s', occ_date = '%s' "
        "WHERE tran_id = %llu AND account_id = '%s'",
        refund->order_tran_id, refund->amount, refund->tran_state,
        refund->query_user, refund->occ_date, refund->tran_id, refund->account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id,
                             const char* account_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !account_id) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_refund_history WHERE tran_id = %llu AND account_id = '%s'",
        tran_id, account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id,
                             const char* account_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !account_id) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM log_refund_history WHERE tran_id = %llu AND account_id = '%s' LIMIT 1",
        tran_id, account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int LogRefundHistory_GetAll(DBConnectionManager* manager, LogRefundHistory* refunds,
                             int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !refunds || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, account_id, order_tran_id, amount, tran_state, "
        "query_user, occ_date FROM log_refund_history ORDER BY occ_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&refunds[count], 0, sizeof(LogRefundHistory));
        refunds[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        if (row[1]) strncpy(refunds[count].account_id, row[1], sizeof(refunds[count].account_id) - 1);
        if (row[2]) strncpy(refunds[count].order_tran_id, row[2], sizeof(refunds[count].order_tran_id) - 1);
        refunds[count].amount = row[3] ? (unsigned int)atoi(row[3]) : 0;
        refunds[count].tran_state = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(refunds[count].query_user, row[5], sizeof(refunds[count].query_user) - 1);
        if (row[6]) strncpy(refunds[count].occ_date, row[6], sizeof(refunds[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                   LogRefundHistory* refunds, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !account_id || !refunds || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, account_id, order_tran_id, amount, tran_state, "
        "query_user, occ_date FROM log_refund_history "
        "WHERE account_id = '%s' ORDER BY occ_date DESC",
        account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&refunds[count], 0, sizeof(LogRefundHistory));
        refunds[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        if (row[1]) strncpy(refunds[count].account_id, row[1], sizeof(refunds[count].account_id) - 1);
        if (row[2]) strncpy(refunds[count].order_tran_id, row[2], sizeof(refunds[count].order_tran_id) - 1);
        refunds[count].amount = row[3] ? (unsigned int)atoi(row[3]) : 0;
        refunds[count].tran_state = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(refunds[count].query_user, row[5], sizeof(refunds[count].query_user) - 1);
        if (row[6]) strncpy(refunds[count].occ_date, row[6], sizeof(refunds[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_GetByState(DBConnectionManager* manager, unsigned char tran_state,
                                 LogRefundHistory* refunds, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !refunds || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, account_id, order_tran_id, amount, tran_state, "
        "query_user, occ_date FROM log_refund_history "
        "WHERE tran_state = %u ORDER BY occ_date DESC",
        tran_state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&refunds[count], 0, sizeof(LogRefundHistory));
        refunds[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        if (row[1]) strncpy(refunds[count].account_id, row[1], sizeof(refunds[count].account_id) - 1);
        if (row[2]) strncpy(refunds[count].order_tran_id, row[2], sizeof(refunds[count].order_tran_id) - 1);
        refunds[count].amount = row[3] ? (unsigned int)atoi(row[3]) : 0;
        refunds[count].tran_state = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(refunds[count].query_user, row[5], sizeof(refunds[count].query_user) - 1);
        if (row[6]) strncpy(refunds[count].occ_date, row[6], sizeof(refunds[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogRefundHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                     const char* end_date, LogRefundHistory* refunds,
                                     int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !start_date || !end_date || !refunds || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, account_id, order_tran_id, amount, tran_state, "
        "query_user, occ_date FROM log_refund_history "
        "WHERE occ_date BETWEEN '%s' AND '%s' ORDER BY occ_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&refunds[count], 0, sizeof(LogRefundHistory));
        refunds[count].tran_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        if (row[1]) strncpy(refunds[count].account_id, row[1], sizeof(refunds[count].account_id) - 1);
        if (row[2]) strncpy(refunds[count].order_tran_id, row[2], sizeof(refunds[count].order_tran_id) - 1);
        refunds[count].amount = row[3] ? (unsigned int)atoi(row[3]) : 0;
        refunds[count].tran_state = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(refunds[count].query_user, row[5], sizeof(refunds[count].query_user) - 1);
        if (row[6]) strncpy(refunds[count].occ_date, row[6], sizeof(refunds[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void LogRefundHistory_PrintInfo(const LogRefundHistory* refund) {
    if (!refund) return;

    printf("=== Log Refund History Info ===\n");
    printf("Transaction ID: %llu\n", refund->tran_id);
    printf("Account ID: %s\n", refund->account_id);
    printf("Order Transaction ID: %s\n", refund->order_tran_id);
    printf("Amount: %u\n", refund->amount);
    printf("Transaction State: %u\n", refund->tran_state);
    printf("Query User: %s\n", refund->query_user);
    printf("Occurred Date: %s\n", refund->occ_date);
    printf("==============================\n");
}
