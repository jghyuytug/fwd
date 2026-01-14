#include "log_charac_money_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogCharacMoney_Add(DBConnectionManager* manager, const LogCharacMoney* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_charac_money (charac_no, occ_date, money_plus, money_minus, m_id) "
        "VALUES (%d, '%s', %d, %d, %d)",
        record->charac_no, record->occ_date, record->money_plus, record->money_minus, record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogCharacMoney_Get(DBConnectionManager* manager, int charac_no, const char* occ_date, LogCharacMoney* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, occ_date, money_plus, money_minus, m_id FROM log_charac_money WHERE charac_no = %d AND occ_date = '%s'",
        charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogCharacMoney));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->money_plus = row[2] ? atoi(row[2]) : 0;
    record->money_minus = row[3] ? atoi(row[3]) : 0;
    record->m_id = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogCharacMoney_GetAll(DBConnectionManager* manager, LogCharacMoney* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, occ_date, money_plus, money_minus, m_id FROM log_charac_money");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogCharacMoney));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].money_plus = row[2] ? atoi(row[2]) : 0;
        records[count].money_minus = row[3] ? atoi(row[3]) : 0;
        records[count].m_id = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
