#include "log_item_refund_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 8192

/* Add new item refund history log (pf_rel_id is auto_increment) */
int LogItemRefundHistory_Add(DBConnectionManager* manager, const LogItemRefundHistory* log) {
    char query[MAX_QUERY_LEN];
    char escaped_reason[512];
    DBQueryResult result;

    if (!manager || !log) return -1;

    /* Escape reason field */
    snprintf(escaped_reason, sizeof(escaped_reason), "%s", log->reason);

    /* Use NOW() if occ_date is empty, otherwise use provided date */
    if (log->occ_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO log_item_refund_history (purchase_tran_id, recharge_tran_id, "
            "account_id, occ_date, reason, admin_id, query_user) VALUES "
            "(%llu, %llu, '%s', '%s', '%s', '%s', '%s')",
            log->purchase_tran_id, log->recharge_tran_id, log->account_id,
            log->occ_date, escaped_reason, log->admin_id, log->query_user);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO log_item_refund_history (purchase_tran_id, recharge_tran_id, "
            "account_id, occ_date, reason, admin_id, query_user) VALUES "
            "(%llu, %llu, '%s', NOW(), '%s', '%s', '%s')",
            log->purchase_tran_id, log->recharge_tran_id, log->account_id,
            escaped_reason, log->admin_id, log->query_user);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Get log by primary key */
int LogItemRefundHistory_Get(DBConnectionManager* manager, unsigned int pf_rel_id,
                              LogItemRefundHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "WHERE pf_rel_id = %u", pf_rel_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(log, 0, sizeof(LogItemRefundHistory));
    log->pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    log->purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
    log->recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
    if (row[3]) strncpy(log->account_id, row[3], sizeof(log->account_id) - 1);
    if (row[4]) strncpy(log->occ_date, row[4], sizeof(log->occ_date) - 1);
    if (row[5]) strncpy(log->reason, row[5], sizeof(log->reason) - 1);
    if (row[6]) strncpy(log->admin_id, row[6], sizeof(log->admin_id) - 1);
    if (row[7]) strncpy(log->query_user, row[7], sizeof(log->query_user) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete log record */
int LogItemRefundHistory_Delete(DBConnectionManager* manager, unsigned int pf_rel_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_item_refund_history WHERE pf_rel_id = %u", pf_rel_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if log exists */
int LogItemRefundHistory_Exists(DBConnectionManager* manager, unsigned int pf_rel_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_item_refund_history WHERE pf_rel_id = %u", pf_rel_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* Get logs by account ID */
int LogItemRefundHistory_GetByAccountId(DBConnectionManager* manager, const char* account_id,
                                        LogItemRefundHistory* logs, int max_count,
                                        int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !account_id || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "WHERE account_id = '%s' ORDER BY occ_date DESC", account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogItemRefundHistory));
        logs[count].pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        logs[count].purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
        logs[count].recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].occ_date, row[4], sizeof(logs[count].occ_date) - 1);
        if (row[5]) strncpy(logs[count].reason, row[5], sizeof(logs[count].reason) - 1);
        if (row[6]) strncpy(logs[count].admin_id, row[6], sizeof(logs[count].admin_id) - 1);
        if (row[7]) strncpy(logs[count].query_user, row[7], sizeof(logs[count].query_user) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get logs by purchase transaction ID */
int LogItemRefundHistory_GetByPurchaseTranId(DBConnectionManager* manager,
                                              unsigned long long purchase_tran_id,
                                              LogItemRefundHistory* logs, int max_count,
                                              int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "WHERE purchase_tran_id = %llu ORDER BY occ_date DESC", purchase_tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogItemRefundHistory));
        logs[count].pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        logs[count].purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
        logs[count].recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].occ_date, row[4], sizeof(logs[count].occ_date) - 1);
        if (row[5]) strncpy(logs[count].reason, row[5], sizeof(logs[count].reason) - 1);
        if (row[6]) strncpy(logs[count].admin_id, row[6], sizeof(logs[count].admin_id) - 1);
        if (row[7]) strncpy(logs[count].query_user, row[7], sizeof(logs[count].query_user) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get logs by recharge transaction ID */
int LogItemRefundHistory_GetByRechargeTranId(DBConnectionManager* manager,
                                              unsigned long long recharge_tran_id,
                                              LogItemRefundHistory* logs, int max_count,
                                              int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "WHERE recharge_tran_id = %llu ORDER BY occ_date DESC", recharge_tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogItemRefundHistory));
        logs[count].pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        logs[count].purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
        logs[count].recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].occ_date, row[4], sizeof(logs[count].occ_date) - 1);
        if (row[5]) strncpy(logs[count].reason, row[5], sizeof(logs[count].reason) - 1);
        if (row[6]) strncpy(logs[count].admin_id, row[6], sizeof(logs[count].admin_id) - 1);
        if (row[7]) strncpy(logs[count].query_user, row[7], sizeof(logs[count].query_user) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get logs by date range */
int LogItemRefundHistory_GetByDateRange(DBConnectionManager* manager,
                                        const char* start_date, const char* end_date,
                                        LogItemRefundHistory* logs, int max_count,
                                        int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !start_date || !end_date || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "WHERE occ_date BETWEEN '%s' AND '%s' ORDER BY occ_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogItemRefundHistory));
        logs[count].pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        logs[count].purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
        logs[count].recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].occ_date, row[4], sizeof(logs[count].occ_date) - 1);
        if (row[5]) strncpy(logs[count].reason, row[5], sizeof(logs[count].reason) - 1);
        if (row[6]) strncpy(logs[count].admin_id, row[6], sizeof(logs[count].admin_id) - 1);
        if (row[7]) strncpy(logs[count].query_user, row[7], sizeof(logs[count].query_user) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get logs by admin ID */
int LogItemRefundHistory_GetByAdmin(DBConnectionManager* manager, const char* admin_id,
                                    LogItemRefundHistory* logs, int max_count,
                                    int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !admin_id || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "WHERE admin_id = '%s' ORDER BY occ_date DESC", admin_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogItemRefundHistory));
        logs[count].pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        logs[count].purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
        logs[count].recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].occ_date, row[4], sizeof(logs[count].occ_date) - 1);
        if (row[5]) strncpy(logs[count].reason, row[5], sizeof(logs[count].reason) - 1);
        if (row[6]) strncpy(logs[count].admin_id, row[6], sizeof(logs[count].admin_id) - 1);
        if (row[7]) strncpy(logs[count].query_user, row[7], sizeof(logs[count].query_user) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get recent logs */
int LogItemRefundHistory_GetRecent(DBConnectionManager* manager, int limit,
                                   LogItemRefundHistory* logs, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pf_rel_id, purchase_tran_id, recharge_tran_id, account_id, "
        "occ_date, reason, admin_id, query_user FROM log_item_refund_history "
        "ORDER BY occ_date DESC LIMIT %d", limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < limit && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogItemRefundHistory));
        logs[count].pf_rel_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        logs[count].purchase_tran_id = row[1] ? (unsigned long long)atoll(row[1]) : 0;
        logs[count].recharge_tran_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].occ_date, row[4], sizeof(logs[count].occ_date) - 1);
        if (row[5]) strncpy(logs[count].reason, row[5], sizeof(logs[count].reason) - 1);
        if (row[6]) strncpy(logs[count].admin_id, row[6], sizeof(logs[count].admin_id) - 1);
        if (row[7]) strncpy(logs[count].query_user, row[7], sizeof(logs[count].query_user) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print log information */
void LogItemRefundHistory_PrintInfo(const LogItemRefundHistory* log) {
    if (!log) return;

    printf("Item Refund History Log:\n");
    printf("  PF Rel ID: %u\n", log->pf_rel_id);
    printf("  Purchase Tran ID: %llu\n", log->purchase_tran_id);
    printf("  Recharge Tran ID: %llu\n", log->recharge_tran_id);
    printf("  Account ID: %s\n", log->account_id);
    printf("  Occurrence Date: %s\n", log->occ_date);
    printf("  Reason: %s\n", log->reason);
    printf("  Admin ID: %s\n", log->admin_id);
    printf("  Query User: %s\n", log->query_user);
}
