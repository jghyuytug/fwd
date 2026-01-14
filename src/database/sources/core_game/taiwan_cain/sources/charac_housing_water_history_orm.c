#include "charac_housing_water_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/* ==================== CRUD 操作实现 ==================== */

int CharacHousingWaterHistory_Add(DBConnectionManager* manager, const CharacHousingWaterHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "INSERT INTO charac_housing_water_history "
        "(charac_no, give_time, give_charac_name) "
        "VALUES (%u, '%s', '%s')",
        record->charac_no, record->give_time, record->give_charac_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingWaterHistory_Get(DBConnectionManager* manager, unsigned int charac_no,
                                   const char* give_time, CharacHousingWaterHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];

    if (!manager || !give_time || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, give_time, give_charac_name "
        "FROM charac_housing_water_history "
        "WHERE charac_no = %u AND give_time = '%s'",
        charac_no, give_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        strncpy(record->give_time, values[1], sizeof(record->give_time) - 1);
        record->give_time[sizeof(record->give_time) - 1] = '\0';
        strncpy(record->give_charac_name, values[2], sizeof(record->give_charac_name) - 1);
        record->give_charac_name[sizeof(record->give_charac_name) - 1] = '\0';
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacHousingWaterHistory_Update(DBConnectionManager* manager, const CharacHousingWaterHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_water_history SET give_charac_name = '%s' "
        "WHERE charac_no = %u AND give_time = '%s'",
        record->give_charac_name, record->charac_no, record->give_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingWaterHistory_Delete(DBConnectionManager* manager, unsigned int charac_no, const char* give_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !give_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_housing_water_history WHERE charac_no = %u AND give_time = '%s'",
        charac_no, give_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingWaterHistory_Exists(DBConnectionManager* manager, unsigned int charac_no, const char* give_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !give_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_housing_water_history "
        "WHERE charac_no = %u AND give_time = '%s'",
        charac_no, give_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return (count > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/* ==================== 查询操作实现 ==================== */

int CharacHousingWaterHistory_CountByCharac(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_housing_water_history WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacHousingWaterHistory_GetByTimeRange(DBConnectionManager* manager, unsigned int charac_no,
                                               const char* start_time, const char* end_time,
                                               CharacHousingWaterHistory* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int count = 0;

    if (!manager || !start_time || !end_time || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, give_time, give_charac_name "
        "FROM charac_housing_water_history "
        "WHERE charac_no = %u AND give_time BETWEEN '%s' AND '%s' "
        "ORDER BY give_time DESC",
        charac_no, start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].give_time, values[1], sizeof(records[count].give_time) - 1);
        records[count].give_time[sizeof(records[count].give_time) - 1] = '\0';
        strncpy(records[count].give_charac_name, values[2], sizeof(records[count].give_charac_name) - 1);
        records[count].give_charac_name[sizeof(records[count].give_charac_name) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacHousingWaterHistory_GetByGiver(DBConnectionManager* manager, const char* give_charac_name,
                                          CharacHousingWaterHistory* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int count = 0;

    if (!manager || !give_charac_name || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, give_time, give_charac_name "
        "FROM charac_housing_water_history "
        "WHERE give_charac_name = '%s' "
        "ORDER BY give_time DESC",
        give_charac_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].give_time, values[1], sizeof(records[count].give_time) - 1);
        records[count].give_time[sizeof(records[count].give_time) - 1] = '\0';
        strncpy(records[count].give_charac_name, values[2], sizeof(records[count].give_charac_name) - 1);
        records[count].give_charac_name[sizeof(records[count].give_charac_name) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacHousingWaterHistory_DeleteAllByCharac(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_housing_water_history WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingWaterHistory_DeleteBefore(DBConnectionManager* manager, const char* before_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !before_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_housing_water_history WHERE give_time < '%s'", before_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingWaterHistory_GetRecentDays(DBConnectionManager* manager, unsigned int charac_no,
                                              int days, CharacHousingWaterHistory* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int count = 0;

    if (!manager || !records || max_count <= 0 || days <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, give_time, give_charac_name "
        "FROM charac_housing_water_history "
        "WHERE charac_no = %u AND give_time >= DATE_SUB(NOW(), INTERVAL %d DAY) "
        "ORDER BY give_time DESC",
        charac_no, days);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].give_time, values[1], sizeof(records[count].give_time) - 1);
        records[count].give_time[sizeof(records[count].give_time) - 1] = '\0';
        strncpy(records[count].give_charac_name, values[2], sizeof(records[count].give_charac_name) - 1);
        records[count].give_charac_name[sizeof(records[count].give_charac_name) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void CharacHousingWaterHistory_PrintInfo(const CharacHousingWaterHistory* record) {
    if (!record) {
        printf("CharacHousingWaterHistory: NULL\n");
        return;
    }

    printf("CharacHousingWaterHistory {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  give_time: %s\n", record->give_time);
    printf("  give_charac_name: %s\n", record->give_charac_name);
    printf("}\n");
}
