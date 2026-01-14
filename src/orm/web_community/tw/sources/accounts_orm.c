#include "accounts_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Accounts_Add(DBConnectionManager* manager, const Accounts* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO accounts (accountname, password) "
        "VALUES ('%s', '%s')",
        record->accountname, record->password);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Accounts_Get(DBConnectionManager* manager, int UID, Accounts* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT UID, accountname, password FROM accounts WHERE UID = %d",
        UID);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Accounts));
    record->UID = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->accountname, row[1], sizeof(record->accountname) - 1);
    if (row[2]) strncpy(record->password, row[2], sizeof(record->password) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Accounts_GetAll(DBConnectionManager* manager, Accounts* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT UID, accountname, password FROM accounts");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Accounts));
        records[count].UID = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].accountname, row[1], sizeof(records[count].accountname) - 1);
        if (row[2]) strncpy(records[count].password, row[2], sizeof(records[count].password) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
