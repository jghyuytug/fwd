#include "log_query_dbmw_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192  /* Increased for TEXT field */

/* ========== CRUD Operations ========== */

int LogQueryDbmwRef_Add(DBConnectionManager* manager, const LogQueryDbmwRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* Note: q_id is auto_increment, not included in INSERT */
    snprintf(query, sizeof(query),
        "INSERT INTO log_query_dbmw_ref (query_hash, query) "
        "VALUES ('%s', '%s')",
        record->query_hash,
        record->query);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_Get(DBConnectionManager* manager,
                         unsigned short q_id,
                         LogQueryDbmwRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT query_hash, q_id, query FROM log_query_dbmw_ref WHERE q_id = %u",
        q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogQueryDbmwRef));
    if (row[0]) {
        strncpy(record->query_hash, row[0], sizeof(record->query_hash) - 1);
        record->query_hash[sizeof(record->query_hash) - 1] = '\0';
    }
    record->q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
    if (row[2]) {
        strncpy(record->query, row[2], sizeof(record->query) - 1);
        record->query[sizeof(record->query) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_GetByHash(DBConnectionManager* manager,
                                const char* query_hash,
                                LogQueryDbmwRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !query_hash || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT query_hash, q_id, query FROM log_query_dbmw_ref WHERE query_hash = '%s'",
        query_hash);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogQueryDbmwRef));
    if (row[0]) {
        strncpy(record->query_hash, row[0], sizeof(record->query_hash) - 1);
        record->query_hash[sizeof(record->query_hash) - 1] = '\0';
    }
    record->q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
    if (row[2]) {
        strncpy(record->query, row[2], sizeof(record->query) - 1);
        record->query[sizeof(record->query) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_Update(DBConnectionManager* manager, const LogQueryDbmwRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE log_query_dbmw_ref SET query_hash = '%s', query = '%s' WHERE q_id = %u",
        record->query_hash,
        record->query,
        record->q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_Delete(DBConnectionManager* manager, unsigned short q_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM log_query_dbmw_ref WHERE q_id = %u", q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_DeleteByHash(DBConnectionManager* manager, const char* query_hash) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !query_hash) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM log_query_dbmw_ref WHERE query_hash = '%s'", query_hash);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_Exists(DBConnectionManager* manager, unsigned short q_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_query_dbmw_ref WHERE q_id = %u", q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

int LogQueryDbmwRef_HashExists(DBConnectionManager* manager, const char* query_hash) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !query_hash) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_query_dbmw_ref WHERE query_hash = '%s'", query_hash);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/* ========== Business Operations ========== */

int LogQueryDbmwRef_GetAll(DBConnectionManager* manager,
                            LogQueryDbmwRef* records,
                            int max_count,
                            int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT query_hash, q_id, query FROM log_query_dbmw_ref ORDER BY q_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogQueryDbmwRef));
        if (row[0]) {
            strncpy(records[count].query_hash, row[0], sizeof(records[count].query_hash) - 1);
            records[count].query_hash[sizeof(records[count].query_hash) - 1] = '\0';
        }
        records[count].q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(records[count].query, row[2], sizeof(records[count].query) - 1);
            records[count].query[sizeof(records[count].query) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_GetLatest(DBConnectionManager* manager, LogQueryDbmwRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT query_hash, q_id, query FROM log_query_dbmw_ref ORDER BY q_id DESC LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogQueryDbmwRef));
    if (row[0]) {
        strncpy(record->query_hash, row[0], sizeof(record->query_hash) - 1);
        record->query_hash[sizeof(record->query_hash) - 1] = '\0';
    }
    record->q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
    if (row[2]) {
        strncpy(record->query, row[2], sizeof(record->query) - 1);
        record->query[sizeof(record->query) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryDbmwRef_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM log_query_dbmw_ref");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

/* ========== Utility Functions ========== */

void LogQueryDbmwRef_PrintInfo(const LogQueryDbmwRef* record) {
    if (!record) {
        printf("LogQueryDbmwRef: NULL\n");
        return;
    }

    printf("LogQueryDbmwRef:\n");
    printf("  query_hash: %s\n", record->query_hash);
    printf("  q_id: %u\n", record->q_id);
    printf("  query: %s\n", record->query);
}
