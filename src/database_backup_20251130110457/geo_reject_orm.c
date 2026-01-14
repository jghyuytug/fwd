#include "geo_reject_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int GeoReject_Add(DBConnectionManager* manager, const GeoReject* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO geo_reject "
        "(rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src) "
        "VALUES ('%s', '%s', %u, '%s', '%c', '%c')",
        record->rej_ip, record->rej_c_code, record->rej_ip_count,
        record->rej_last_date, record->rej_chk, record->rej_src);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoReject_Get(DBConnectionManager* manager, const char* rej_ip, GeoReject* record) {
    if (!manager || !rej_ip || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject WHERE rej_ip = '%s'",
        rej_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        strncpy(record->rej_ip, values[0], sizeof(record->rej_ip) - 1);
        strncpy(record->rej_c_code, values[1], sizeof(record->rej_c_code) - 1);
        record->rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(record->rej_last_date, values[3], sizeof(record->rej_last_date) - 1);
        record->rej_chk = values[4][0];
        record->rej_src = values[5][0];
        record->rej_ip[sizeof(record->rej_ip) - 1] = '\0';
        record->rej_c_code[sizeof(record->rej_c_code) - 1] = '\0';
        record->rej_last_date[sizeof(record->rej_last_date) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GeoReject_Update(DBConnectionManager* manager, const GeoReject* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE geo_reject SET rej_c_code = '%s', rej_ip_count = %u, "
        "rej_last_date = '%s', rej_chk = '%c', rej_src = '%c' "
        "WHERE rej_ip = '%s'",
        record->rej_c_code, record->rej_ip_count, record->rej_last_date,
        record->rej_chk, record->rej_src, record->rej_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoReject_Delete(DBConnectionManager* manager, const char* rej_ip) {
    if (!manager || !rej_ip) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM geo_reject WHERE rej_ip = '%s'", rej_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoReject_Exists(DBConnectionManager* manager, const char* rej_ip) {
    if (!manager || !rej_ip) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM geo_reject WHERE rej_ip = '%s'", rej_ip);

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

int GeoReject_GetByCountryCode(DBConnectionManager* manager, const char* country_code,
                                 GeoReject* records, int max_count) {
    if (!manager || !country_code || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject WHERE rej_c_code = '%s' LIMIT %d",
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
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetBySource(DBConnectionManager* manager, char source,
                           GeoReject* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject WHERE rej_src = '%c' LIMIT %d",
        source, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetByCheckFlag(DBConnectionManager* manager, char check_flag,
                               GeoReject* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject WHERE rej_chk = '%c' LIMIT %d",
        check_flag, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetByCountRange(DBConnectionManager* manager, int min_count, int max_count_val,
                                GeoReject* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject WHERE rej_ip_count BETWEEN %d AND %d LIMIT %d",
        min_count, max_count_val, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetByIPPattern(DBConnectionManager* manager, const char* ip_pattern,
                               GeoReject* records, int max_count) {
    if (!manager || !ip_pattern || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject WHERE rej_ip LIKE '%s' LIMIT %d",
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
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetRecentUpdated(DBConnectionManager* manager, GeoReject* records, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject ORDER BY rej_last_date DESC LIMIT %d",
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
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetAll(DBConnectionManager* manager, GeoReject* records, int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src "
        "FROM geo_reject LIMIT %d, %d",
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
        strncpy(records[count].rej_ip, values[0], sizeof(records[count].rej_ip) - 1);
        strncpy(records[count].rej_c_code, values[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = (unsigned int)atoi(values[2]);
        strncpy(records[count].rej_last_date, values[3], sizeof(records[count].rej_last_date) - 1);
        records[count].rej_chk = values[4][0];
        records[count].rej_src = values[5][0];
        records[count].rej_ip[sizeof(records[count].rej_ip) - 1] = '\0';
        records[count].rej_c_code[sizeof(records[count].rej_c_code) - 1] = '\0';
        records[count].rej_last_date[sizeof(records[count].rej_last_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GeoReject_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM geo_reject");

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

int GeoReject_BulkAdd(DBConnectionManager* manager, const GeoReject* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (GeoReject_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int GeoReject_BulkDelete(DBConnectionManager* manager, const char** ip_list, int count) {
    if (!manager || !ip_list || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (GeoReject_Delete(manager, ip_list[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void GeoReject_PrintInfo(const GeoReject* record) {
    if (!record) {
        printf("GeoReject is NULL\n");
        return;
    }

    printf("=== Geo Reject ===\n");
    printf("  rej_ip:        %s\n", record->rej_ip);
    printf("  rej_c_code:    %s\n", record->rej_c_code);
    printf("  rej_ip_count:  %u\n", record->rej_ip_count);
    printf("  rej_last_date: %s\n", record->rej_last_date);
    printf("  rej_chk:       %c\n", record->rej_chk);
    printf("  rej_src:       %c\n", record->rej_src);
    printf("==================\n");
}
