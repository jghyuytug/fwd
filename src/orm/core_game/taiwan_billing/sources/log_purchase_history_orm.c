#include "log_purchase_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations */

int LogPurchaseHistory_Add(DBConnectionManager* manager, const LogPurchaseHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->occ_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO log_purchase_history (tran_id, tran_state, account_id, charac_id, "
            "item_id, cera, befor_cera, after_cera, query_user, occ_date) "
            "VALUES (%"PRIu64", %u, '%s', '%s', %u, %u, %u, %u, '%s', '%s')",
            record->tran_id, record->tran_state, record->account_id, record->charac_id,
            record->item_id, record->cera, record->befor_cera, record->after_cera,
            record->query_user, record->occ_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO log_purchase_history (tran_id, tran_state, account_id, charac_id, "
            "item_id, cera, befor_cera, after_cera, query_user, occ_date) "
            "VALUES (%"PRIu64", %u, '%s', '%s', %u, %u, %u, %u, '%s', NOW())",
            record->tran_id, record->tran_state, record->account_id, record->charac_id,
            record->item_id, record->cera, record->befor_cera, record->after_cera,
            record->query_user);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_Get(DBConnectionManager* manager, uint64_t tran_id, LogPurchaseHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history WHERE tran_id = %"PRIu64, tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogPurchaseHistory));
    record->tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
    record->tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
    if (row[2]) strncpy(record->account_id, row[2], sizeof(record->account_id) - 1);
    if (row[3]) strncpy(record->charac_id, row[3], sizeof(record->charac_id) - 1);
    record->item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
    record->cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
    record->befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
    record->after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) strncpy(record->query_user, row[8], sizeof(record->query_user) - 1);
    if (row[9]) strncpy(record->occ_date, row[9], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_Delete(DBConnectionManager* manager, uint64_t tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_purchase_history WHERE tran_id = %"PRIu64, tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_Exists(DBConnectionManager* manager, uint64_t tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_purchase_history WHERE tran_id = %"PRIu64, tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Business Operations */

int LogPurchaseHistory_GetRecent(DBConnectionManager* manager, LogPurchaseHistory* records,
                                  int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history ORDER BY tran_id DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPurchaseHistory));
        records[count].tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
        records[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].account_id, row[2], sizeof(records[count].account_id) - 1);
        if (row[3]) strncpy(records[count].charac_id, row[3], sizeof(records[count].charac_id) - 1);
        records[count].item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        records[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        records[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        records[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].query_user, row[8], sizeof(records[count].query_user) - 1);
        if (row[9]) strncpy(records[count].occ_date, row[9], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                     LogPurchaseHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !account_id || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history WHERE account_id = '%s' "
        "ORDER BY tran_id DESC LIMIT %d", account_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPurchaseHistory));
        records[count].tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
        records[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].account_id, row[2], sizeof(records[count].account_id) - 1);
        if (row[3]) strncpy(records[count].charac_id, row[3], sizeof(records[count].charac_id) - 1);
        records[count].item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        records[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        records[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        records[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].query_user, row[8], sizeof(records[count].query_user) - 1);
        if (row[9]) strncpy(records[count].occ_date, row[9], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetByCharacter(DBConnectionManager* manager, const char* charac_id,
                                       LogPurchaseHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !charac_id || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history WHERE charac_id = '%s' "
        "ORDER BY tran_id DESC LIMIT %d", charac_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPurchaseHistory));
        records[count].tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
        records[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].account_id, row[2], sizeof(records[count].account_id) - 1);
        if (row[3]) strncpy(records[count].charac_id, row[3], sizeof(records[count].charac_id) - 1);
        records[count].item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        records[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        records[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        records[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].query_user, row[8], sizeof(records[count].query_user) - 1);
        if (row[9]) strncpy(records[count].occ_date, row[9], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetByItem(DBConnectionManager* manager, unsigned int item_id,
                                  LogPurchaseHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history WHERE item_id = %u "
        "ORDER BY tran_id DESC LIMIT %d", item_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPurchaseHistory));
        records[count].tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
        records[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].account_id, row[2], sizeof(records[count].account_id) - 1);
        if (row[3]) strncpy(records[count].charac_id, row[3], sizeof(records[count].charac_id) - 1);
        records[count].item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        records[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        records[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        records[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].query_user, row[8], sizeof(records[count].query_user) - 1);
        if (row[9]) strncpy(records[count].occ_date, row[9], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetByDateRange(DBConnectionManager* manager,
                                        const char* start_date, const char* end_date,
                                        LogPurchaseHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !start_date || !end_date || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history WHERE occ_date BETWEEN '%s' AND '%s' "
        "ORDER BY tran_id DESC LIMIT %d", start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPurchaseHistory));
        records[count].tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
        records[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].account_id, row[2], sizeof(records[count].account_id) - 1);
        if (row[3]) strncpy(records[count].charac_id, row[3], sizeof(records[count].charac_id) - 1);
        records[count].item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        records[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        records[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        records[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].query_user, row[8], sizeof(records[count].query_user) - 1);
        if (row[9]) strncpy(records[count].occ_date, row[9], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetByState(DBConnectionManager* manager, unsigned char tran_state,
                                    LogPurchaseHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, tran_state, account_id, charac_id, item_id, cera, "
        "befor_cera, after_cera, query_user, occ_date "
        "FROM log_purchase_history WHERE tran_state = %u "
        "ORDER BY tran_id DESC LIMIT %d", tran_state, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPurchaseHistory));
        records[count].tran_id = row[0] ? (uint64_t)strtoull(row[0], NULL, 10) : 0;
        records[count].tran_state = row[1] ? (unsigned char)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].account_id, row[2], sizeof(records[count].account_id) - 1);
        if (row[3]) strncpy(records[count].charac_id, row[3], sizeof(records[count].charac_id) - 1);
        records[count].item_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        records[count].cera = row[5] ? (unsigned int)atoi(row[5]) : 0;
        records[count].befor_cera = row[6] ? (unsigned int)atoi(row[6]) : 0;
        records[count].after_cera = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].query_user, row[8], sizeof(records[count].query_user) - 1);
        if (row[9]) strncpy(records[count].occ_date, row[9], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetTotalSpent(DBConnectionManager* manager, const char* account_id,
                                      unsigned int* total_cera) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !account_id || !total_cera) return -1;

    snprintf(query, sizeof(query),
        "SELECT SUM(cera) FROM log_purchase_history WHERE account_id = '%s'", account_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *total_cera = row[0] ? (unsigned int)atoi(row[0]) : 0;
    } else {
        *total_cera = 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogPurchaseHistory_GetCount(DBConnectionManager* manager, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_purchase_history");

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

void LogPurchaseHistory_PrintInfo(const LogPurchaseHistory* record) {
    if (!record) return;

    printf("Tran ID: %"PRIu64"\n", record->tran_id);
    printf("State: %u\n", record->tran_state);
    printf("Account: %s\n", record->account_id);
    printf("Character: %s\n", record->charac_id);
    printf("Item ID: %u\n", record->item_id);
    printf("Cera Spent: %u\n", record->cera);
    printf("Before Cera: %u\n", record->befor_cera);
    printf("After Cera: %u\n", record->after_cera);
    printf("Query User: %s\n", record->query_user);
    printf("Occ Date: %s\n", record->occ_date);
}
