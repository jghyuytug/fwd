#include "geo_allow_country_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int GeoAllowCountry_Add(DBConnectionManager* manager, const GeoAllowCountry* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO geo_allow_country "
        "(server_group, country_code, reg_date) "
        "VALUES (%d, '%s', '%s')",
        record->server_group, record->country_code, record->reg_date);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllowCountry_Get(DBConnectionManager* manager, int server_group,
                         const char* country_code, GeoAllowCountry* record) {
    if (!manager || !country_code || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country WHERE server_group = %d AND country_code = '%s'",
        server_group, country_code);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->server_group = atoi(values[0]);
        strncpy(record->country_code, values[1], sizeof(record->country_code) - 1);
        strncpy(record->reg_date, values[2], sizeof(record->reg_date) - 1);
        record->country_code[sizeof(record->country_code) - 1] = '\0';
        record->reg_date[sizeof(record->reg_date) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GeoAllowCountry_Update(DBConnectionManager* manager, const GeoAllowCountry* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE geo_allow_country SET reg_date = '%s' "
        "WHERE server_group = %d AND country_code = '%s'",
        record->reg_date, record->server_group, record->country_code);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllowCountry_Delete(DBConnectionManager* manager, int server_group,
                            const char* country_code) {
    if (!manager || !country_code) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM geo_allow_country WHERE server_group = %d AND country_code = '%s'",
        server_group, country_code);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllowCountry_Exists(DBConnectionManager* manager, int server_group,
                            const char* country_code) {
    if (!manager || !country_code) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM geo_allow_country WHERE server_group = %d AND country_code = '%s'",
        server_group, country_code);

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

int GeoAllowCountry_GetByServerGroup(DBConnectionManager* manager, int server_group,
                                      GeoAllowCountry* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country WHERE server_group = %d LIMIT %d",
        server_group, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].server_group = atoi(values[0]);
        strncpy(records[count].country_code, values[1], sizeof(records[count].country_code) - 1);
        strncpy(records[count].reg_date, values[2], sizeof(records[count].reg_date) - 1);
        records[count].country_code[sizeof(records[count].country_code) - 1] = '\0';
        records[count].reg_date[sizeof(records[count].reg_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllowCountry_GetByCountryCode(DBConnectionManager* manager, const char* country_code,
                                      GeoAllowCountry* records, int max_count) {
    if (!manager || !country_code || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country WHERE country_code = '%s' LIMIT %d",
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
        records[count].server_group = atoi(values[0]);
        strncpy(records[count].country_code, values[1], sizeof(records[count].country_code) - 1);
        strncpy(records[count].reg_date, values[2], sizeof(records[count].reg_date) - 1);
        records[count].country_code[sizeof(records[count].country_code) - 1] = '\0';
        records[count].reg_date[sizeof(records[count].reg_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllowCountry_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, GeoAllowCountry* records, int max_count) {
    if (!manager || !start_date || !end_date || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country WHERE reg_date BETWEEN '%s' AND '%s' LIMIT %d",
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
        records[count].server_group = atoi(values[0]);
        strncpy(records[count].country_code, values[1], sizeof(records[count].country_code) - 1);
        strncpy(records[count].reg_date, values[2], sizeof(records[count].reg_date) - 1);
        records[count].country_code[sizeof(records[count].country_code) - 1] = '\0';
        records[count].reg_date[sizeof(records[count].reg_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllowCountry_GetRecentAdded(DBConnectionManager* manager, GeoAllowCountry* records,
                                    int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country ORDER BY reg_date DESC LIMIT %d",
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
        records[count].server_group = atoi(values[0]);
        strncpy(records[count].country_code, values[1], sizeof(records[count].country_code) - 1);
        strncpy(records[count].reg_date, values[2], sizeof(records[count].reg_date) - 1);
        records[count].country_code[sizeof(records[count].country_code) - 1] = '\0';
        records[count].reg_date[sizeof(records[count].reg_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllowCountry_GetAll(DBConnectionManager* manager, GeoAllowCountry* records,
                            int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country LIMIT %d, %d",
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
        records[count].server_group = atoi(values[0]);
        strncpy(records[count].country_code, values[1], sizeof(records[count].country_code) - 1);
        strncpy(records[count].reg_date, values[2], sizeof(records[count].reg_date) - 1);
        records[count].country_code[sizeof(records[count].country_code) - 1] = '\0';
        records[count].reg_date[sizeof(records[count].reg_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoAllowCountry_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM geo_allow_country");

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

int GeoAllowCountry_GetTop(DBConnectionManager* manager, GeoAllowCountry* records, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date "
        "FROM geo_allow_country ORDER BY server_group ASC, country_code ASC LIMIT %d",
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
        records[count].server_group = atoi(values[0]);
        strncpy(records[count].country_code, values[1], sizeof(records[count].country_code) - 1);
        strncpy(records[count].reg_date, values[2], sizeof(records[count].reg_date) - 1);
        records[count].country_code[sizeof(records[count].country_code) - 1] = '\0';
        records[count].reg_date[sizeof(records[count].reg_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int GeoAllowCountry_BulkAdd(DBConnectionManager* manager, const GeoAllowCountry* records,
                             int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (GeoAllowCountry_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int GeoAllowCountry_BulkDelete(DBConnectionManager* manager, const GeoAllowCountry* records,
                                int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (GeoAllowCountry_Delete(manager, records[i].server_group, records[i].country_code) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void GeoAllowCountry_PrintInfo(const GeoAllowCountry* record) {
    if (!record) {
        printf("GeoAllowCountry is NULL\n");
        return;
    }

    printf("=== Geo Allow Country ===\n");
    printf("  server_group:  %d\n", record->server_group);
    printf("  country_code:  %s\n", record->country_code);
    printf("  reg_date:      %s\n", record->reg_date);
    printf("=========================\n");
}
