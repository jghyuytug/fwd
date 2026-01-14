#include "account_cargo_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AccountCargoMod_Add(DBConnectionManager* manager, const AccountCargoMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO account_cargo_mod (m_id, server_info, money_old, cargo_capa_old, money_new, cargo_capa_new, mng_user_id, work_time, work_type, memo) "
        "VALUES (%d, %d, %d, %d, %d, %d, '%s', '%s', %d, '%s')",
        record->m_id, record->server_info, record->money_old, record->cargo_capa_old, record->money_new, record->cargo_capa_new, record->mng_user_id, record->work_time, record->work_type, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCargoMod_Get(DBConnectionManager* manager, int id, AccountCargoMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_info, money_old, cargo_capa_old, money_new, cargo_capa_new, mng_user_id, work_time, work_type, memo FROM account_cargo_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AccountCargoMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_info = row[2] ? atoi(row[2]) : 0;
    record->money_old = row[3] ? atoi(row[3]) : 0;
    record->cargo_capa_old = row[4] ? atoi(row[4]) : 0;
    record->money_new = row[5] ? atoi(row[5]) : 0;
    record->cargo_capa_new = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->mng_user_id, row[7], sizeof(record->mng_user_id) - 1);
    if (row[8]) strncpy(record->work_time, row[8], sizeof(record->work_time) - 1);
    record->work_type = row[9] ? row[9][0] : '\0';
    if (row[10]) strncpy(record->memo, row[10], sizeof(record->memo) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCargoMod_GetAll(DBConnectionManager* manager, AccountCargoMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_info, money_old, cargo_capa_old, money_new, cargo_capa_new, mng_user_id, work_time, work_type, memo FROM account_cargo_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AccountCargoMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_info = row[2] ? atoi(row[2]) : 0;
        records[count].money_old = row[3] ? atoi(row[3]) : 0;
        records[count].cargo_capa_old = row[4] ? atoi(row[4]) : 0;
        records[count].money_new = row[5] ? atoi(row[5]) : 0;
        records[count].cargo_capa_new = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].mng_user_id, row[7], sizeof(records[count].mng_user_id) - 1);
        if (row[8]) strncpy(records[count].work_time, row[8], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[9] ? row[9][0] : '\0';
        if (row[10]) strncpy(records[count].memo, row[10], sizeof(records[count].memo) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
