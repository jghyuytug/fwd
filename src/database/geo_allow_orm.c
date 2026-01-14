#include "geo_allow_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int GeoAllow_Add(DBConnectionManager* manager, const GeoAllow* allow) {
    if (!manager || !allow) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO geo_allow "
        "(allow_ip, allow_c_code, allow_date) "
        "VALUES ('%s', '%s', '%s')",
        allow->allow_ip, allow->allow_c_code, allow->allow_date);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllow_Get(DBConnectionManager* manager, const char* allow_ip, GeoAllow* allow) {
    if (!manager || !allow_ip || !allow) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow WHERE allow_ip = '%s'",
        allow_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        strncpy(allow->allow_ip, values[0], sizeof(allow->allow_ip) - 1);
        strncpy(allow->allow_c_code, values[1], sizeof(allow->allow_c_code) - 1);
        strncpy(allow->allow_date, values[2], sizeof(allow->allow_date) - 1);
        allow->allow_ip[sizeof(allow->allow_ip) - 1] = '\0';
        allow->allow_c_code[sizeof(allow->allow_c_code) - 1] = '\0';
        allow->allow_date[sizeof(allow->allow_date) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GeoAllow_Update(DBConnectionManager* manager, const GeoAllow* allow) {
    if (!manager || !allow) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE geo_allow SET "
        "allow_c_code = '%s', allow_date = '%s' "
        "WHERE allow_ip = '%s'",
        allow->allow_c_code, allow->allow_date, allow->allow_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllow_Delete(DBConnectionManager* manager, const char* allow_ip) {
    if (!manager || !allow_ip) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM geo_allow WHERE allow_ip = '%s'", allow_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllow_Exists(DBConnectionManager* manager, const char* allow_ip) {
    if (!manager || !allow_ip) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM geo_allow WHERE allow_ip = '%s'", allow_ip);

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

int GeoAllow_GetByCountryCode(DBConnectionManager* manager, const char* country_code,
                               GeoAllow* allows, int max_count) {
    if (!manager || !country_code || !allows || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow WHERE allow_c_code = '%s' LIMIT %d",
        country_code, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(allows[count].allow_ip, values[0], sizeof(allows[count].allow_ip) - 1);
        strncpy(allows[count].allow_c_code, values[1], sizeof(allows[count].allow_c_code) - 1);
        strncpy(allows[count].allow_date, values[2], sizeof(allows[count].allow_date) - 1);
        allows[count].allow_ip[sizeof(allows[count].allow_ip) - 1] = '\0';
        allows[count].allow_c_code[sizeof(allows[count].allow_c_code) - 1] = '\0';
        allows[count].allow_date[sizeof(allows[count].allow_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllow_GetByIPPattern(DBConnectionManager* manager, const char* ip_pattern,
                             GeoAllow* allows, int max_count) {
    if (!manager || !ip_pattern || !allows || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow WHERE allow_ip LIKE '%s' LIMIT %d",
        ip_pattern, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(allows[count].allow_ip, values[0], sizeof(allows[count].allow_ip) - 1);
        strncpy(allows[count].allow_c_code, values[1], sizeof(allows[count].allow_c_code) - 1);
        strncpy(allows[count].allow_date, values[2], sizeof(allows[count].allow_date) - 1);
        allows[count].allow_ip[sizeof(allows[count].allow_ip) - 1] = '\0';
        allows[count].allow_c_code[sizeof(allows[count].allow_c_code) - 1] = '\0';
        allows[count].allow_date[sizeof(allows[count].allow_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllow_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                             const char* end_date, GeoAllow* allows, int max_count) {
    if (!manager || !start_date || !end_date || !allows || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow WHERE allow_date BETWEEN '%s' AND '%s' LIMIT %d",
        start_date, end_date, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(allows[count].allow_ip, values[0], sizeof(allows[count].allow_ip) - 1);
        strncpy(allows[count].allow_c_code, values[1], sizeof(allows[count].allow_c_code) - 1);
        strncpy(allows[count].allow_date, values[2], sizeof(allows[count].allow_date) - 1);
        allows[count].allow_ip[sizeof(allows[count].allow_ip) - 1] = '\0';
        allows[count].allow_c_code[sizeof(allows[count].allow_c_code) - 1] = '\0';
        allows[count].allow_date[sizeof(allows[count].allow_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllow_GetAll(DBConnectionManager* manager, GeoAllow* allows, int offset, int limit) {
    if (!manager || !allows || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow LIMIT %d, %d",
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
        strncpy(allows[count].allow_ip, values[0], sizeof(allows[count].allow_ip) - 1);
        strncpy(allows[count].allow_c_code, values[1], sizeof(allows[count].allow_c_code) - 1);
        strncpy(allows[count].allow_date, values[2], sizeof(allows[count].allow_date) - 1);
        allows[count].allow_ip[sizeof(allows[count].allow_ip) - 1] = '\0';
        allows[count].allow_c_code[sizeof(allows[count].allow_c_code) - 1] = '\0';
        allows[count].allow_date[sizeof(allows[count].allow_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllow_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM geo_allow");

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

int GeoAllow_GetRecent(DBConnectionManager* manager, GeoAllow* allows, int limit) {
    if (!manager || !allows || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow ORDER BY allow_date DESC LIMIT %d",
        limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(allows[count].allow_ip, values[0], sizeof(allows[count].allow_ip) - 1);
        strncpy(allows[count].allow_c_code, values[1], sizeof(allows[count].allow_c_code) - 1);
        strncpy(allows[count].allow_date, values[2], sizeof(allows[count].allow_date) - 1);
        allows[count].allow_ip[sizeof(allows[count].allow_ip) - 1] = '\0';
        allows[count].allow_c_code[sizeof(allows[count].allow_c_code) - 1] = '\0';
        allows[count].allow_date[sizeof(allows[count].allow_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllow_GetOldRecords(DBConnectionManager* manager, int days_ago,
                            GeoAllow* allows, int max_count) {
    if (!manager || !allows || max_count <= 0 || days_ago < 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date "
        "FROM geo_allow WHERE allow_date < DATE_SUB(NOW(), INTERVAL %d DAY) LIMIT %d",
        days_ago, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(allows[count].allow_ip, values[0], sizeof(allows[count].allow_ip) - 1);
        strncpy(allows[count].allow_c_code, values[1], sizeof(allows[count].allow_c_code) - 1);
        strncpy(allows[count].allow_date, values[2], sizeof(allows[count].allow_date) - 1);
        allows[count].allow_ip[sizeof(allows[count].allow_ip) - 1] = '\0';
        allows[count].allow_c_code[sizeof(allows[count].allow_c_code) - 1] = '\0';
        allows[count].allow_date[sizeof(allows[count].allow_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int GeoAllow_BulkAdd(DBConnectionManager* manager, const GeoAllow* allows, int count) {
    if (!manager || !allows || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (GeoAllow_Add(manager, &allows[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int GeoAllow_BulkDelete(DBConnectionManager* manager, const char** allow_ips, int count) {
    if (!manager || !allow_ips || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (GeoAllow_Delete(manager, allow_ips[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void GeoAllow_PrintInfo(const GeoAllow* allow) {
    if (!allow) {
        printf("GeoAllow is NULL\n");
        return;
    }

    printf("=== Geo Allow ===\n");
    printf("  allow_ip:     %s\n", allow->allow_ip);
    printf("  country_code: %s\n", allow->allow_c_code);
    printf("  allow_date:   %s\n", allow->allow_date);
    printf("=================\n");
}
