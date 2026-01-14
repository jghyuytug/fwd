#include "test_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int Test_Add(DBConnectionManager* manager, const TestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->a_is_null && record->b_is_null) {
        snprintf(query, sizeof(query), "INSERT INTO test (a, b) VALUES (NULL, NULL)");
    } else if (record->a_is_null) {
        snprintf(query, sizeof(query), "INSERT INTO test (a, b) VALUES (NULL, '%s')",
                 record->b);
    } else if (record->b_is_null) {
        snprintf(query, sizeof(query), "INSERT INTO test (a, b) VALUES (%d, NULL)",
                 record->a);
    } else {
        snprintf(query, sizeof(query), "INSERT INTO test (a, b) VALUES (%d, '%s')",
                 record->a, record->b);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Test_GetAll(DBConnectionManager* manager, TestRecord* records,
                int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query), "SELECT a, b FROM test LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TestRecord));

        if (row[0]) {
            records[count].a = atoi(row[0]);
            records[count].a_is_null = 0;
        } else {
            records[count].a = 0;
            records[count].a_is_null = 1;
        }

        if (row[1]) {
            strncpy(records[count].b, row[1], sizeof(records[count].b) - 1);
            records[count].b_is_null = 0;
        } else {
            records[count].b[0] = '\0';
            records[count].b_is_null = 1;
        }

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int Test_Count(DBConnectionManager* manager, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM test");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

int Test_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM test");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Test_GetByA(DBConnectionManager* manager, int a, TestRecord* records,
                int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
             "SELECT a, b FROM test WHERE a = %d LIMIT %d", a, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TestRecord));

        if (row[0]) {
            records[count].a = atoi(row[0]);
            records[count].a_is_null = 0;
        } else {
            records[count].a = 0;
            records[count].a_is_null = 1;
        }

        if (row[1]) {
            strncpy(records[count].b, row[1], sizeof(records[count].b) - 1);
            records[count].b_is_null = 0;
        } else {
            records[count].b[0] = '\0';
            records[count].b_is_null = 1;
        }

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int Test_DeleteByA(DBConnectionManager* manager, int a) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM test WHERE a = %d", a);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void Test_PrintInfo(const TestRecord* record) {
    if (!record) return;

    printf("=== Test Record ===\n");
    if (record->a_is_null) {
        printf("a: NULL\n");
    } else {
        printf("a: %d\n", record->a);
    }

    if (record->b_is_null) {
        printf("b: NULL\n");
    } else {
        printf("b: %s\n", record->b);
    }
    printf("===================\n");
}
