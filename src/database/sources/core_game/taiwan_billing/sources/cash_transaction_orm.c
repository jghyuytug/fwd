#include "cash_transaction_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add new cash transaction record (tran_id is auto_increment) */
int CashTransaction_Add(DBConnectionManager* manager, const CashTransaction* transaction) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !transaction) return -1;

    /* tran_id is auto_increment, so we only insert dummy */
    snprintf(query, sizeof(query),
        "INSERT INTO cash_transaction (dummy) VALUES ('%c')",
        transaction->dummy);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Get cash transaction by tran_id */
int CashTransaction_Get(DBConnectionManager* manager, long long tran_id,
                        CashTransaction* transaction) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !transaction) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, dummy FROM cash_transaction WHERE tran_id = %lld", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(transaction, 0, sizeof(CashTransaction));
    transaction->tran_id = row[0] ? atoll(row[0]) : 0;
    transaction->dummy = row[1] ? row[1][0] : '\0';

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete cash transaction record */
int CashTransaction_Delete(DBConnectionManager* manager, long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM cash_transaction WHERE tran_id = %lld", tran_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if cash transaction exists */
int CashTransaction_Exists(DBConnectionManager* manager, long long tran_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM cash_transaction WHERE tran_id = %lld", tran_id);

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

/* Get next available transaction ID */
int CashTransaction_GetNextTranId(DBConnectionManager* manager, long long* next_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !next_id) return -1;

    /* Get MAX(tran_id) + 1, or 1 if table is empty */
    snprintf(query, sizeof(query),
        "SELECT COALESCE(MAX(tran_id), 0) + 1 FROM cash_transaction");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *next_id = row[0] ? atoll(row[0]) : 1;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get latest transaction ID */
int CashTransaction_GetLatestTranId(DBConnectionManager* manager, long long* latest_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !latest_id) return -1;

    snprintf(query, sizeof(query),
        "SELECT MAX(tran_id) FROM cash_transaction");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* If table is empty, MAX returns NULL */
    *latest_id = (row[0] && row[0][0] != '\0') ? atoll(row[0]) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get transactions in a range */
int CashTransaction_GetRange(DBConnectionManager* manager, long long start_id,
                             long long end_id, CashTransaction* transactions,
                             int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !transactions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, dummy FROM cash_transaction "
        "WHERE tran_id BETWEEN %lld AND %lld ORDER BY tran_id",
        start_id, end_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&transactions[count], 0, sizeof(CashTransaction));
        transactions[count].tran_id = row[0] ? atoll(row[0]) : 0;
        transactions[count].dummy = row[1] ? row[1][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get all transactions */
int CashTransaction_GetAll(DBConnectionManager* manager, CashTransaction* transactions,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !transactions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT tran_id, dummy FROM cash_transaction ORDER BY tran_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&transactions[count], 0, sizeof(CashTransaction));
        transactions[count].tran_id = row[0] ? atoll(row[0]) : 0;
        transactions[count].dummy = row[1] ? row[1][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print transaction information */
void CashTransaction_PrintInfo(const CashTransaction* transaction) {
    if (!transaction) return;

    printf("Cash Transaction Record:\n");
    printf("  Transaction ID: %lld\n", transaction->tran_id);
    printf("  Dummy: %c\n", transaction->dummy);
}
