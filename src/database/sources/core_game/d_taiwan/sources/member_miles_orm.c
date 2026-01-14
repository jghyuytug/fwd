#include "member_miles_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberMiles_Add(DBConnectionManager* manager, const MemberMiles* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_miles (m_id, miles, daily_miles) VALUES (%d, %d, %d)",
        record->m_id, record->miles, record->daily_miles);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMiles_Get(DBConnectionManager* manager, int m_id, MemberMiles* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, miles, daily_miles FROM member_miles WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->m_id = atoi(values[0]);
        record->miles = atoi(values[1]);
        record->daily_miles = (short)atoi(values[2]);
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberMiles_Update(DBConnectionManager* manager, const MemberMiles* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_miles SET miles = %d, daily_miles = %d WHERE m_id = %d",
        record->miles, record->daily_miles, record->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMiles_Delete(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_miles WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMiles_Exists(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_miles WHERE m_id = %d",
        m_id);

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

int MemberMiles_GetByMilesRange(DBConnectionManager* manager, int min_miles,
                                  int max_miles, MemberMiles* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, miles, daily_miles FROM member_miles "
        "WHERE miles BETWEEN %d AND %d LIMIT %d",
        min_miles, max_miles, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        records[count].miles = atoi(values[1]);
        records[count].daily_miles = (short)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMiles_GetByDailyMilesRange(DBConnectionManager* manager, short min_daily,
                                       short max_daily, MemberMiles* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, miles, daily_miles FROM member_miles "
        "WHERE daily_miles BETWEEN %d AND %d LIMIT %d",
        min_daily, max_daily, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        records[count].miles = atoi(values[1]);
        records[count].daily_miles = (short)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMiles_GetTopMiles(DBConnectionManager* manager, MemberMiles* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, miles, daily_miles FROM member_miles "
        "ORDER BY miles DESC LIMIT %d",
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
        records[count].m_id = atoi(values[0]);
        records[count].miles = atoi(values[1]);
        records[count].daily_miles = (short)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMiles_GetAll(DBConnectionManager* manager, MemberMiles* records,
                        int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, miles, daily_miles FROM member_miles LIMIT %d, %d",
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
        records[count].m_id = atoi(values[0]);
        records[count].miles = atoi(values[1]);
        records[count].daily_miles = (short)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMiles_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_miles");

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

int MemberMiles_IncrementMiles(DBConnectionManager* manager, int m_id, int miles_to_add) {
    if (!manager || miles_to_add < 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_miles SET miles = miles + %d WHERE m_id = %d",
        miles_to_add, m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMiles_ResetDailyMiles(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_miles SET daily_miles = 0 WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ======================================== Bulk Operations ======================================== */

int MemberMiles_BulkAdd(DBConnectionManager* manager, const MemberMiles* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberMiles_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberMiles_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_miles WHERE m_id = %d", m_ids[i]);

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

void MemberMiles_PrintInfo(const MemberMiles* record) {
    if (!record) {
        printf("MemberMiles is NULL\n");
        return;
    }

    printf("=== Member Miles Info ===\n");
    printf("  m_id:         %d\n", record->m_id);
    printf("  miles:        %d\n", record->miles);
    printf("  daily_miles:  %d\n", record->daily_miles);
    printf("=========================\n");
}
