#include "member_abnomal_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberAbnomal_Add(DBConnectionManager* manager, const MemberAbnomal* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_abnomal "
        "(user_id, overlab_count) "
        "VALUES ('%s', %d)",
        record->user_id, record->overlab_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberAbnomal_Get(DBConnectionManager* manager, const char* user_id, MemberAbnomal* record) {
    if (!manager || !user_id || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count "
        "FROM member_abnomal WHERE user_id = '%s'",
        user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        strncpy(record->user_id, values[0], sizeof(record->user_id) - 1);
        record->user_id[sizeof(record->user_id) - 1] = '\0';
        record->overlab_count = (short)atoi(values[1]);
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberAbnomal_Update(DBConnectionManager* manager, const MemberAbnomal* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_abnomal SET overlab_count = %d "
        "WHERE user_id = '%s'",
        record->overlab_count, record->user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberAbnomal_Delete(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_abnomal WHERE user_id = '%s'", user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberAbnomal_Exists(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_abnomal WHERE user_id = '%s'", user_id);

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

/* ======================================== Business Queries ======================================== */

int MemberAbnomal_GetByCountRange(DBConnectionManager* manager, short min_count, short max_count,
                                   MemberAbnomal* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count "
        "FROM member_abnomal WHERE overlab_count BETWEEN %d AND %d LIMIT %d",
        min_count, max_count, max_records);

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
        records[count].overlab_count = (short)atoi(values[1]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberAbnomal_GetHighCount(DBConnectionManager* manager, short min_count,
                                MemberAbnomal* records, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count "
        "FROM member_abnomal WHERE overlab_count >= %d "
        "ORDER BY overlab_count DESC LIMIT %d",
        min_count, limit);

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
        records[count].overlab_count = (short)atoi(values[1]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberAbnomal_GetByUserIdPattern(DBConnectionManager* manager, const char* pattern,
                                      MemberAbnomal* records, int max_records) {
    if (!manager || !pattern || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count "
        "FROM member_abnomal WHERE user_id LIKE '%s' LIMIT %d",
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
        records[count].overlab_count = (short)atoi(values[1]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberAbnomal_IncrementCount(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_abnomal SET overlab_count = overlab_count + 1 "
        "WHERE user_id = '%s'",
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

int MemberAbnomal_ResetCount(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_abnomal SET overlab_count = 0 "
        "WHERE user_id = '%s'",
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

int MemberAbnomal_GetAll(DBConnectionManager* manager, MemberAbnomal* records,
                         int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT user_id, overlab_count "
        "FROM member_abnomal LIMIT %d, %d",
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
        records[count].overlab_count = (short)atoi(values[1]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberAbnomal_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_abnomal");

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

/* ======================================== Bulk Operations ======================================== */

int MemberAbnomal_BulkAdd(DBConnectionManager* manager, const MemberAbnomal* records,
                          int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberAbnomal_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberAbnomal_BulkDelete(DBConnectionManager* manager, const char** user_ids,
                              int count) {
    if (!manager || !user_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (MemberAbnomal_Delete(manager, user_ids[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberAbnomal_PrintInfo(const MemberAbnomal* record) {
    if (!record) {
        printf("MemberAbnomal is NULL\n");
        return;
    }

    printf("=== Member Abnomal Info ===\n");
    printf("  user_id:        %s\n", record->user_id);
    printf("  overlab_count:  %d\n", record->overlab_count);
    printf("===========================\n");
}
