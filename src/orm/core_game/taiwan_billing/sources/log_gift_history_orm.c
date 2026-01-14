#include "log_gift_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogGiftHistory_Add(DBConnectionManager* manager, const LogGiftHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_gift_history (tran_id, tran_state, send_account_id, "
        "send_charac_id, recv_account_id, item_id, cera, send_befor_cera, "
        "send_after_cera, recv_befor_cera, recv_after_cera, query_user, occ_date) "
        "VALUES (%llu, %u, '%s', '%s', '%s', %u, %u, %u, %u, %u, %u, '%s', '%s')",
        log->tran_id, log->tran_state, log->send_account_id, log->send_charac_id,
        log->recv_account_id, log->item_id, log->cera, log->send_befor_cera,
        log->send_after_cera, log->recv_befor_cera, log->recv_after_cera,
        log->query_user, log->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_Get(DBConnectionManager* manager, unsigned long long tran_id, LogGiftHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, send_account_id, send_charac_id, recv_account_id, "
        "item_id, cera, send_befor_cera, send_after_cera, recv_befor_cera, "
        "recv_after_cera, query_user, occ_date FROM log_gift_history WHERE tran_id = %llu",
        tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(log, 0, sizeof(LogGiftHistory));
    log->tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
    log->tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
    if (row[2]) strncpy(log->send_account_id, row[2], sizeof(log->send_account_id) - 1);
    if (row[3]) strncpy(log->send_charac_id, row[3], sizeof(log->send_charac_id) - 1);
    if (row[4]) strncpy(log->recv_account_id, row[4], sizeof(log->recv_account_id) - 1);
    log->item_id = row[5] ? (unsigned int)atoi(row[5]) : 0;
    log->cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
    log->send_befor_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
    log->send_after_cera = row[8] ? (unsigned int)atoi(row[8]) : 0;
    log->recv_befor_cera = row[9] ? (unsigned int)atoi(row[9]) : 0;
    log->recv_after_cera = row[10] ? (unsigned int)atoi(row[10]) : 0;
    if (row[11]) strncpy(log->query_user, row[11], sizeof(log->query_user) - 1);
    if (row[12]) strncpy(log->occ_date, row[12], sizeof(log->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_Update(DBConnectionManager* manager, const LogGiftHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_gift_history SET tran_state = %u, send_account_id = '%s', "
        "send_charac_id = '%s', recv_account_id = '%s', item_id = %u, cera = %u, "
        "send_befor_cera = %u, send_after_cera = %u, recv_befor_cera = %u, "
        "recv_after_cera = %u, query_user = '%s', occ_date = '%s' WHERE tran_id = %llu",
        log->tran_state, log->send_account_id, log->send_charac_id, log->recv_account_id,
        log->item_id, log->cera, log->send_befor_cera, log->send_after_cera,
        log->recv_befor_cera, log->recv_after_cera, log->query_user, log->occ_date,
        log->tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_gift_history WHERE tran_id = %llu", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_gift_history WHERE tran_id = %llu", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int LogGiftHistory_GetAll(DBConnectionManager* manager, LogGiftHistory* logs,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, send_account_id, send_charac_id, recv_account_id, "
        "item_id, cera, send_befor_cera, send_after_cera, recv_befor_cera, "
        "recv_after_cera, query_user, occ_date FROM log_gift_history ORDER BY occ_date DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogGiftHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        logs[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].send_account_id, row[2], sizeof(logs[count].send_account_id) - 1);
        if (row[3]) strncpy(logs[count].send_charac_id, row[3], sizeof(logs[count].send_charac_id) - 1);
        if (row[4]) strncpy(logs[count].recv_account_id, row[4], sizeof(logs[count].recv_account_id) - 1);
        logs[count].item_id = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].send_befor_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].send_after_cera = row[8] ? (unsigned int)atoi(row[8]) : 0;
        logs[count].recv_befor_cera = row[9] ? (unsigned int)atoi(row[9]) : 0;
        logs[count].recv_after_cera = row[10] ? (unsigned int)atoi(row[10]) : 0;
        if (row[11]) strncpy(logs[count].query_user, row[11], sizeof(logs[count].query_user) - 1);
        if (row[12]) strncpy(logs[count].occ_date, row[12], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                 LogGiftHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !account_id || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, send_account_id, send_charac_id, recv_account_id, "
        "item_id, cera, send_befor_cera, send_after_cera, recv_befor_cera, "
        "recv_after_cera, query_user, occ_date FROM log_gift_history "
        "WHERE send_account_id = '%s' OR recv_account_id = '%s' ORDER BY occ_date DESC LIMIT %d",
        account_id, account_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogGiftHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        logs[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].send_account_id, row[2], sizeof(logs[count].send_account_id) - 1);
        if (row[3]) strncpy(logs[count].send_charac_id, row[3], sizeof(logs[count].send_charac_id) - 1);
        if (row[4]) strncpy(logs[count].recv_account_id, row[4], sizeof(logs[count].recv_account_id) - 1);
        logs[count].item_id = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].send_befor_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].send_after_cera = row[8] ? (unsigned int)atoi(row[8]) : 0;
        logs[count].recv_befor_cera = row[9] ? (unsigned int)atoi(row[9]) : 0;
        logs[count].recv_after_cera = row[10] ? (unsigned int)atoi(row[10]) : 0;
        if (row[11]) strncpy(logs[count].query_user, row[11], sizeof(logs[count].query_user) - 1);
        if (row[12]) strncpy(logs[count].occ_date, row[12], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_GetByState(DBConnectionManager* manager, unsigned char state,
                               LogGiftHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, send_account_id, send_charac_id, recv_account_id, "
        "item_id, cera, send_befor_cera, send_after_cera, recv_befor_cera, "
        "recv_after_cera, query_user, occ_date FROM log_gift_history "
        "WHERE tran_state = %u ORDER BY occ_date DESC LIMIT %d",
        state, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogGiftHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        logs[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].send_account_id, row[2], sizeof(logs[count].send_account_id) - 1);
        if (row[3]) strncpy(logs[count].send_charac_id, row[3], sizeof(logs[count].send_charac_id) - 1);
        if (row[4]) strncpy(logs[count].recv_account_id, row[4], sizeof(logs[count].recv_account_id) - 1);
        logs[count].item_id = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].send_befor_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].send_after_cera = row[8] ? (unsigned int)atoi(row[8]) : 0;
        logs[count].recv_befor_cera = row[9] ? (unsigned int)atoi(row[9]) : 0;
        logs[count].recv_after_cera = row[10] ? (unsigned int)atoi(row[10]) : 0;
        if (row[11]) strncpy(logs[count].query_user, row[11], sizeof(logs[count].query_user) - 1);
        if (row[12]) strncpy(logs[count].occ_date, row[12], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogGiftHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                   const char* end_date, LogGiftHistory* logs,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !start_date || !end_date || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, send_account_id, send_charac_id, recv_account_id, "
        "item_id, cera, send_befor_cera, send_after_cera, recv_befor_cera, "
        "recv_after_cera, query_user, occ_date FROM log_gift_history "
        "WHERE occ_date BETWEEN '%s' AND '%s' ORDER BY occ_date DESC LIMIT %d",
        start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogGiftHistory));
        logs[count].tran_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        logs[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(logs[count].send_account_id, row[2], sizeof(logs[count].send_account_id) - 1);
        if (row[3]) strncpy(logs[count].send_charac_id, row[3], sizeof(logs[count].send_charac_id) - 1);
        if (row[4]) strncpy(logs[count].recv_account_id, row[4], sizeof(logs[count].recv_account_id) - 1);
        logs[count].item_id = row[5] ? (unsigned int)atoi(row[5]) : 0;
        logs[count].cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        logs[count].send_befor_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        logs[count].send_after_cera = row[8] ? (unsigned int)atoi(row[8]) : 0;
        logs[count].recv_befor_cera = row[9] ? (unsigned int)atoi(row[9]) : 0;
        logs[count].recv_after_cera = row[10] ? (unsigned int)atoi(row[10]) : 0;
        if (row[11]) strncpy(logs[count].query_user, row[11], sizeof(logs[count].query_user) - 1);
        if (row[12]) strncpy(logs[count].occ_date, row[12], sizeof(logs[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void LogGiftHistory_PrintInfo(const LogGiftHistory* log) {
    if (!log) {
        printf("LogGiftHistory: NULL\n");
        return;
    }

    printf("LogGiftHistory:\n");
    printf("  tran_id: %llu\n", log->tran_id);
    printf("  tran_state: %u\n", log->tran_state);
    printf("  send_account_id: %s\n", log->send_account_id);
    printf("  send_charac_id: %s\n", log->send_charac_id);
    printf("  recv_account_id: %s\n", log->recv_account_id);
    printf("  item_id: %u\n", log->item_id);
    printf("  cera: %u\n", log->cera);
    printf("  send_befor_cera: %u\n", log->send_befor_cera);
    printf("  send_after_cera: %u\n", log->send_after_cera);
    printf("  recv_befor_cera: %u\n", log->recv_befor_cera);
    printf("  recv_after_cera: %u\n", log->recv_after_cera);
    printf("  query_user: %s\n", log->query_user);
    printf("  occ_date: %s\n", log->occ_date);
}
