#include "geo_country_code_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int GeoCountryCode_Add(DBConnectionManager* manager, const GeoCountryCode* code) {
    if (!manager || !code) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO geo_country_code "
        "(code_no, country_code_a2, country_code_a3, country) "
        "VALUES (%d, '%s', '%s', '%s')",
        code->code_no, code->country_code_a2, code->country_code_a3, code->country);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoCountryCode_Get(DBConnectionManager* manager, int code_no, GeoCountryCode* code) {
    if (!manager || !code) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code WHERE code_no = %d",
        code_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        code->code_no = atoi(values[0]);
        strncpy(code->country_code_a2, values[1], sizeof(code->country_code_a2) - 1);
        strncpy(code->country_code_a3, values[2], sizeof(code->country_code_a3) - 1);
        strncpy(code->country, values[3], sizeof(code->country) - 1);
        code->country_code_a2[sizeof(code->country_code_a2) - 1] = '\0';
        code->country_code_a3[sizeof(code->country_code_a3) - 1] = '\0';
        code->country[sizeof(code->country) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GeoCountryCode_Update(DBConnectionManager* manager, const GeoCountryCode* code) {
    if (!manager || !code) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE geo_country_code SET "
        "country_code_a2 = '%s', country_code_a3 = '%s', country = '%s' "
        "WHERE code_no = %d",
        code->country_code_a2, code->country_code_a3, code->country, code->code_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoCountryCode_Delete(DBConnectionManager* manager, int code_no) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM geo_country_code WHERE code_no = %d", code_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoCountryCode_Exists(DBConnectionManager* manager, int code_no) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM geo_country_code WHERE code_no = %d", code_no);

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

int GeoCountryCode_GetByA2(DBConnectionManager* manager, const char* country_code_a2,
                            GeoCountryCode* code) {
    if (!manager || !country_code_a2 || !code) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code WHERE country_code_a2 = '%s'",
        country_code_a2);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        code->code_no = atoi(values[0]);
        strncpy(code->country_code_a2, values[1], sizeof(code->country_code_a2) - 1);
        strncpy(code->country_code_a3, values[2], sizeof(code->country_code_a3) - 1);
        strncpy(code->country, values[3], sizeof(code->country) - 1);
        code->country_code_a2[sizeof(code->country_code_a2) - 1] = '\0';
        code->country_code_a3[sizeof(code->country_code_a3) - 1] = '\0';
        code->country[sizeof(code->country) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GeoCountryCode_GetByA3(DBConnectionManager* manager, const char* country_code_a3,
                            GeoCountryCode* code) {
    if (!manager || !country_code_a3 || !code) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code WHERE country_code_a3 = '%s'",
        country_code_a3);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        code->code_no = atoi(values[0]);
        strncpy(code->country_code_a2, values[1], sizeof(code->country_code_a2) - 1);
        strncpy(code->country_code_a3, values[2], sizeof(code->country_code_a3) - 1);
        strncpy(code->country, values[3], sizeof(code->country) - 1);
        code->country_code_a2[sizeof(code->country_code_a2) - 1] = '\0';
        code->country_code_a3[sizeof(code->country_code_a3) - 1] = '\0';
        code->country[sizeof(code->country) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GeoCountryCode_GetByCountryPattern(DBConnectionManager* manager, const char* country_pattern,
                                        GeoCountryCode* codes, int max_count) {
    if (!manager || !country_pattern || !codes || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code WHERE country LIKE '%%%s%%' LIMIT %d",
        country_pattern, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        codes[count].code_no = atoi(values[0]);
        strncpy(codes[count].country_code_a2, values[1], sizeof(codes[count].country_code_a2) - 1);
        strncpy(codes[count].country_code_a3, values[2], sizeof(codes[count].country_code_a3) - 1);
        strncpy(codes[count].country, values[3], sizeof(codes[count].country) - 1);
        codes[count].country_code_a2[sizeof(codes[count].country_code_a2) - 1] = '\0';
        codes[count].country_code_a3[sizeof(codes[count].country_code_a3) - 1] = '\0';
        codes[count].country[sizeof(codes[count].country) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoCountryCode_GetByCodeRange(DBConnectionManager* manager, int start_code, int end_code,
                                   GeoCountryCode* codes, int max_count) {
    if (!manager || !codes || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code WHERE code_no BETWEEN %d AND %d LIMIT %d",
        start_code, end_code, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        codes[count].code_no = atoi(values[0]);
        strncpy(codes[count].country_code_a2, values[1], sizeof(codes[count].country_code_a2) - 1);
        strncpy(codes[count].country_code_a3, values[2], sizeof(codes[count].country_code_a3) - 1);
        strncpy(codes[count].country, values[3], sizeof(codes[count].country) - 1);
        codes[count].country_code_a2[sizeof(codes[count].country_code_a2) - 1] = '\0';
        codes[count].country_code_a3[sizeof(codes[count].country_code_a3) - 1] = '\0';
        codes[count].country[sizeof(codes[count].country) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoCountryCode_GetAll(DBConnectionManager* manager, GeoCountryCode* codes,
                           int offset, int limit) {
    if (!manager || !codes || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code LIMIT %d, %d",
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
        codes[count].code_no = atoi(values[0]);
        strncpy(codes[count].country_code_a2, values[1], sizeof(codes[count].country_code_a2) - 1);
        strncpy(codes[count].country_code_a3, values[2], sizeof(codes[count].country_code_a3) - 1);
        strncpy(codes[count].country, values[3], sizeof(codes[count].country) - 1);
        codes[count].country_code_a2[sizeof(codes[count].country_code_a2) - 1] = '\0';
        codes[count].country_code_a3[sizeof(codes[count].country_code_a3) - 1] = '\0';
        codes[count].country[sizeof(codes[count].country) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoCountryCode_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM geo_country_code");

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

int GeoCountryCode_GetTop(DBConnectionManager* manager, GeoCountryCode* codes, int limit) {
    if (!manager || !codes || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country "
        "FROM geo_country_code ORDER BY code_no ASC LIMIT %d",
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
        codes[count].code_no = atoi(values[0]);
        strncpy(codes[count].country_code_a2, values[1], sizeof(codes[count].country_code_a2) - 1);
        strncpy(codes[count].country_code_a3, values[2], sizeof(codes[count].country_code_a3) - 1);
        strncpy(codes[count].country, values[3], sizeof(codes[count].country) - 1);
        codes[count].country_code_a2[sizeof(codes[count].country_code_a2) - 1] = '\0';
        codes[count].country_code_a3[sizeof(codes[count].country_code_a3) - 1] = '\0';
        codes[count].country[sizeof(codes[count].country) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int GeoCountryCode_BulkAdd(DBConnectionManager* manager, const GeoCountryCode* codes, int count) {
    if (!manager || !codes || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (GeoCountryCode_Add(manager, &codes[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int GeoCountryCode_BulkDelete(DBConnectionManager* manager, const int* code_nos, int count) {
    if (!manager || !code_nos || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (GeoCountryCode_Delete(manager, code_nos[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void GeoCountryCode_PrintInfo(const GeoCountryCode* code) {
    if (!code) {
        printf("GeoCountryCode is NULL\n");
        return;
    }

    printf("=== Geo Country Code ===\n");
    printf("  code_no:         %d\n", code->code_no);
    printf("  country_code_a2: %s\n", code->country_code_a2);
    printf("  country_code_a3: %s\n", code->country_code_a3);
    printf("  country:         %s\n", code->country);
    printf("========================\n");
}
