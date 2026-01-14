#include "cash_cera_point_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations */

int CashCeraPoint_Add(DBConnectionManager* manager, const CashCeraPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    /* Handle datetime fields */
    if (record->reg_date[0] && record->mod_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO cash_cera_point (account, cera_point, reg_date, mod_date) "
            "VALUES ('%s', %u, '%s', '%s')",
            record->account, record->cera_point, record->reg_date, record->mod_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO cash_cera_point (account, cera_point, reg_date, mod_date) "
            "VALUES ('%s', %u, NOW(), NOW())",
            record->account, record->cera_point);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_Get(DBConnectionManager* manager, const char* account, CashCeraPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !account || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT account, cera_point, reg_date, mod_date "
        "FROM cash_cera_point WHERE account = '%s'", account);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CashCeraPoint));
    if (row[0]) strncpy(record->account, row[0], sizeof(record->account) - 1);
    record->cera_point = row[1] ? (unsigned int)atoi(row[1]) : 0;
    if (row[2]) strncpy(record->reg_date, row[2], sizeof(record->reg_date) - 1);
    if (row[3]) strncpy(record->mod_date, row[3], sizeof(record->mod_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_Update(DBConnectionManager* manager, const CashCeraPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->mod_date[0]) {
        snprintf(query, sizeof(query),
            "UPDATE cash_cera_point SET cera_point = %u, mod_date = '%s' "
            "WHERE account = '%s'",
            record->cera_point, record->mod_date, record->account);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE cash_cera_point SET cera_point = %u, mod_date = NOW() "
            "WHERE account = '%s'",
            record->cera_point, record->account);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_Delete(DBConnectionManager* manager, const char* account) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !account) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM cash_cera_point WHERE account = '%s'", account);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_Exists(DBConnectionManager* manager, const char* account) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !account) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM cash_cera_point WHERE account = '%s'", account);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Business Operations */

int CashCeraPoint_AddPoints(DBConnectionManager* manager, const char* account, unsigned int amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !account) return -1;

    snprintf(query, sizeof(query),
        "UPDATE cash_cera_point SET cera_point = cera_point + %u, mod_date = NOW() "
        "WHERE account = '%s'",
        amount, account);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_DeductPoints(DBConnectionManager* manager, const char* account, unsigned int amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    unsigned int current_balance = 0;

    if (!manager || !account) return -1;

    /* Check balance first */
    if (CashCeraPoint_GetBalance(manager, account, &current_balance) < 0)
        return -1;

    if (current_balance < amount)
        return -1;  /* Insufficient balance */

    snprintf(query, sizeof(query),
        "UPDATE cash_cera_point SET cera_point = cera_point - %u, mod_date = NOW() "
        "WHERE account = '%s'",
        amount, account);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_GetBalance(DBConnectionManager* manager, const char* account, unsigned int* balance) {
    CashCeraPoint record;

    if (!manager || !account || !balance) return -1;

    if (CashCeraPoint_Get(manager, account, &record) < 0)
        return -1;

    *balance = record.cera_point;
    return 0;
}

int CashCeraPoint_GetAll(DBConnectionManager* manager, CashCeraPoint* records,
                         int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT account, cera_point, reg_date, mod_date "
        "FROM cash_cera_point ORDER BY account");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CashCeraPoint));
        if (row[0]) strncpy(records[count].account, row[0], sizeof(records[count].account) - 1);
        records[count].cera_point = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].reg_date, row[2], sizeof(records[count].reg_date) - 1);
        if (row[3]) strncpy(records[count].mod_date, row[3], sizeof(records[count].mod_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int CashCeraPoint_GetByPointsRange(DBConnectionManager* manager,
                                    unsigned int min_points, unsigned int max_points,
                                    CashCeraPoint* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT account, cera_point, reg_date, mod_date "
        "FROM cash_cera_point WHERE cera_point BETWEEN %u AND %u "
        "ORDER BY cera_point DESC",
        min_points, max_points);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CashCeraPoint));
        if (row[0]) strncpy(records[count].account, row[0], sizeof(records[count].account) - 1);
        records[count].cera_point = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].reg_date, row[2], sizeof(records[count].reg_date) - 1);
        if (row[3]) strncpy(records[count].mod_date, row[3], sizeof(records[count].mod_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Utility Functions */

void CashCeraPoint_PrintInfo(const CashCeraPoint* record) {
    if (!record) return;

    printf("Account: %s\n", record->account);
    printf("Cera Points: %u\n", record->cera_point);
    printf("Reg Date: %s\n", record->reg_date);
    printf("Mod Date: %s\n", record->mod_date);
}
