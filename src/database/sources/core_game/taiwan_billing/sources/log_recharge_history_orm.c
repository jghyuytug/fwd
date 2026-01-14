#include "log_recharge_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogRechargeHistory_Add(DBConnectionManager* manager, const LogRechargeHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_recharge_history (tran_id, order_tran_id, tran_state, "
        "account_id, charac_id, cera, befor_cera, after_cera, charge_type, "
        "query_user, occ_date) VALUES (%llu, '%s', %u, '%s', '%s', %u, %u, %u, %u, '%s', '%s')",
        log->tran_id, log->order_tran_id, log->tran_state, log->account_id,
        log->charac_id, log->cera, log->befor_cera, log->after_cera,
        log->charge_type, log->query_user, log->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_Get(DBConnectionManager* manager, unsigned long long tran_id, LogRechargeHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, order_tran_id, tran_state, account_id, charac_id, cera, "
        "befor_cera, after_cera, charge_type, query_user, occ_date "
        "FROM log_recharge_history WHERE tran_id = %llu", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(log, 0, sizeof(LogRechargeHistory));
    log->tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
    if (row[1]) strncpy(log->order_tran_id, row[1], sizeof(log->order_tran_id) - 1);
    log->tran_state = row[2] ? (unsigned char)atoi(row[2]) : 0;
    if (row[3]) strncpy(log->account_id, row[3], sizeof(log->account_id) - 1);
    if (row[4]) strncpy(log->charac_id, row[4], sizeof(log->charac_id) - 1);
    log->cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
    log->befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
    log->after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
    log->charge_type = row[8] ? (unsigned char)atoi(row[8]) : 0;
    if (row[9]) strncpy(log->query_user, row[9], sizeof(log->query_user) - 1);
    if (row[10]) strncpy(log->occ_date, row[10], sizeof(log->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_Update(DBConnectionManager* manager, const LogRechargeHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_recharge_history SET order_tran_id = '%s', tran_state = %u, "
        "account_id = '%s', charac_id = '%s', cera = %u, befor_cera = %u, "
        "after_cera = %u, charge_type = %u, query_user = '%s', occ_date = '%s' "
        "WHERE tran_id = %llu",
        log->order_tran_id, log->tran_state, log->account_id, log->charac_id,
        log->cera, log->befor_cera, log->after_cera, log->charge_type,
        log->query_user, log->occ_date, log->tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_recharge_history WHERE tran_id = %llu", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_recharge_history WHERE tran_id = %llu", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int LogRechargeHistory_GetAll(DBConnectionManager* manager, LogRechargeHistory* logs,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, order_tran_id, tran_state, account_id, charac_id, cera, "
        "befor_cera, after_cera, charge_type, query_user, occ_date "
        "FROM log_recharge_history ORDER BY occ_date DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogRechargeHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        if (row[1]) strncpy(logs[count].order_tran_id, row[1], sizeof(logs[count].order_tran_id) - 1);
        logs[count].tran_state = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].charac_id, row[4], sizeof(logs[count].charac_id) - 1);
        logs[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].charge_type = row[8] ? (unsigned char)atoi(row[8]) : 0;
        if (row[9]) strncpy(logs[count].query_user, row[9], sizeof(logs[count].query_user) - 1);
        if (row[10]) strncpy(logs[count].occ_date, row[10], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                     LogRechargeHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !account_id || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, order_tran_id, tran_state, account_id, charac_id, cera, "
        "befor_cera, after_cera, charge_type, query_user, occ_date "
        "FROM log_recharge_history WHERE account_id = '%s' ORDER BY occ_date DESC LIMIT %d",
        account_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogRechargeHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        if (row[1]) strncpy(logs[count].order_tran_id, row[1], sizeof(logs[count].order_tran_id) - 1);
        logs[count].tran_state = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].charac_id, row[4], sizeof(logs[count].charac_id) - 1);
        logs[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].charge_type = row[8] ? (unsigned char)atoi(row[8]) : 0;
        if (row[9]) strncpy(logs[count].query_user, row[9], sizeof(logs[count].query_user) - 1);
        if (row[10]) strncpy(logs[count].occ_date, row[10], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_GetByCharac(DBConnectionManager* manager, const char* charac_id,
                                    LogRechargeHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !charac_id || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, order_tran_id, tran_state, account_id, charac_id, cera, "
        "befor_cera, after_cera, charge_type, query_user, occ_date "
        "FROM log_recharge_history WHERE charac_id = '%s' ORDER BY occ_date DESC LIMIT %d",
        charac_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogRechargeHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        if (row[1]) strncpy(logs[count].order_tran_id, row[1], sizeof(logs[count].order_tran_id) - 1);
        logs[count].tran_state = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].charac_id, row[4], sizeof(logs[count].charac_id) - 1);
        logs[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].charge_type = row[8] ? (unsigned char)atoi(row[8]) : 0;
        if (row[9]) strncpy(logs[count].query_user, row[9], sizeof(logs[count].query_user) - 1);
        if (row[10]) strncpy(logs[count].occ_date, row[10], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogRechargeHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                       const char* end_date, LogRechargeHistory* logs,
                                       int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !start_date || !end_date || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, order_tran_id, tran_state, account_id, charac_id, cera, "
        "befor_cera, after_cera, charge_type, query_user, occ_date "
        "FROM log_recharge_history WHERE occ_date BETWEEN '%s' AND '%s' "
        "ORDER BY occ_date DESC LIMIT %d", start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogRechargeHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        if (row[1]) strncpy(logs[count].order_tran_id, row[1], sizeof(logs[count].order_tran_id) - 1);
        logs[count].tran_state = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(logs[count].account_id, row[3], sizeof(logs[count].account_id) - 1);
        if (row[4]) strncpy(logs[count].charac_id, row[4], sizeof(logs[count].charac_id) - 1);
        logs[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].charge_type = row[8] ? (unsigned char)atoi(row[8]) : 0;
        if (row[9]) strncpy(logs[count].query_user, row[9], sizeof(logs[count].query_user) - 1);
        if (row[10]) strncpy(logs[count].occ_date, row[10], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void LogRechargeHistory_PrintInfo(const LogRechargeHistory* log) {
    if (!log) {
        printf("LogRechargeHistory: NULL\n");
        return;
    }

    printf("LogRechargeHistory:\n");
    printf("  tran_id: %llu\n", log->tran_id);
    printf("  order_tran_id: %s\n", log->order_tran_id);
    printf("  tran_state: %u\n", log->tran_state);
    printf("  account_id: %s\n", log->account_id);
    printf("  charac_id: %s\n", log->charac_id);
    printf("  cera: %u\n", log->cera);
    printf("  befor_cera: %u\n", log->befor_cera);
    printf("  after_cera: %u\n", log->after_cera);
    printf("  charge_type: %u\n", log->charge_type);
    printf("  query_user: %s\n", log->query_user);
    printf("  occ_date: %s\n", log->occ_date);
}
