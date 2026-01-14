#include "prod_buy_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdBuyUser_Add(DBConnectionManager* manager, const ProdBuyUser* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_buy_user (m_id, user_id, sex, birthday, first_buy_time, last_buy_time) "
        "VALUES (%d, '%s', %d, '%s', '%s', '%s')",
        record->m_id, record->user_id, record->sex, record->birthday, record->first_buy_time, record->last_buy_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdBuyUser_Get(DBConnectionManager* manager, int m_id, ProdBuyUser* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, sex, birthday, first_buy_time, last_buy_time FROM prod_buy_user WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdBuyUser));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->user_id, row[1], sizeof(record->user_id) - 1);
    record->sex = row[2] ? row[2][0] : '\0';
    if (row[3]) strncpy(record->birthday, row[3], sizeof(record->birthday) - 1);
    if (row[4]) strncpy(record->first_buy_time, row[4], sizeof(record->first_buy_time) - 1);
    if (row[5]) strncpy(record->last_buy_time, row[5], sizeof(record->last_buy_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ProdBuyUser_GetAll(DBConnectionManager* manager, ProdBuyUser* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, sex, birthday, first_buy_time, last_buy_time FROM prod_buy_user");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdBuyUser));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].user_id, row[1], sizeof(records[count].user_id) - 1);
        records[count].sex = row[2] ? row[2][0] : '\0';
        if (row[3]) strncpy(records[count].birthday, row[3], sizeof(records[count].birthday) - 1);
        if (row[4]) strncpy(records[count].first_buy_time, row[4], sizeof(records[count].first_buy_time) - 1);
        if (row[5]) strncpy(records[count].last_buy_time, row[5], sizeof(records[count].last_buy_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
