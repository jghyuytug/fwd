#include "member_lioness_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberLioness_Add(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_lioness (user_id) VALUES ('%s')",
        user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLioness_Delete(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_lioness WHERE user_id = '%s'",
        user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLioness_Exists(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_lioness WHERE user_id = '%s'",
        user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int MemberLioness_GetAll(DBConnectionManager* manager, MemberLioness* records,
                          int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness LIMIT %d, %d",
        offset, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_lioness");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Business Queries ======================================== */

int MemberLioness_GetByPattern(DBConnectionManager* manager, const char* pattern,
                                 MemberLioness* records, int max_records) {
    if (!manager || !pattern || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness WHERE user_id LIKE '%s' LIMIT %d",
        pattern, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetByPrefix(DBConnectionManager* manager, const char* prefix,
                                MemberLioness* records, int max_records) {
    if (!manager || !prefix || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness WHERE user_id LIKE '%s%%' LIMIT %d",
        prefix, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetBySuffix(DBConnectionManager* manager, const char* suffix,
                                MemberLioness* records, int max_records) {
    if (!manager || !suffix || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness WHERE user_id LIKE '%%%s' LIMIT %d",
        suffix, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetContaining(DBConnectionManager* manager, const char* substring,
                                  MemberLioness* records, int max_records) {
    if (!manager || !substring || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness WHERE user_id LIKE '%%%s%%' LIMIT %d",
        substring, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetByLengthRange(DBConnectionManager* manager, int min_length,
                                     int max_length, MemberLioness* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness WHERE LENGTH(user_id) BETWEEN %d AND %d LIMIT %d",
        min_length, max_length, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetRandom(DBConnectionManager* manager, MemberLioness* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness ORDER BY RAND() LIMIT %d",
        max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLioness_GetSorted(DBConnectionManager* manager, MemberLioness* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id FROM member_lioness ORDER BY user_id ASC LIMIT %d",
        max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].user_id, values[0], sizeof(records[count].user_id) - 1);
        records[count].user_id[sizeof(records[count].user_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int MemberLioness_BulkAdd(DBConnectionManager* manager, const char** user_ids, int count) {
    if (!manager || !user_ids || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberLioness_Add(manager, user_ids[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberLioness_BulkDelete(DBConnectionManager* manager, const char** user_ids, int count) {
    if (!manager || !user_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_lioness WHERE user_id = '%s'", user_ids[i]);

        DBQueryResult result;
        memset(&result, 0, sizeof(DBQueryResult));

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberLioness_PrintInfo(const MemberLioness* record) {
    if (!record) {
        printf("MemberLioness is NULL\n");
        return;
    }

    printf("=== Member Lioness Info ===\n");
    printf("  user_id: %s\n", record->user_id);
    printf("===========================\n");
}
