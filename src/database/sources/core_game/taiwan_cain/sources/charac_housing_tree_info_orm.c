#include "charac_housing_tree_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/* ==================== CRUD 操作实现 ==================== */

int CharacHousingTreeInfo_Add(DBConnectionManager* manager, const CharacHousingTreeInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_housing_tree_info "
        "(charac_no, tree_id, expire_date, current_point, leaf_point, day_water_count) "
        "VALUES (%u, %u, '%s', %d, %d, %d)",
        record->charac_no, record->tree_id, record->expire_date,
        record->current_point, record->leaf_point, record->day_water_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacHousingTreeInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[6];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, tree_id, expire_date, current_point, leaf_point, day_water_count "
        "FROM charac_housing_tree_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->tree_id = (unsigned int)atoi(values[1]);
        strncpy(record->expire_date, values[2], sizeof(record->expire_date) - 1);
        record->expire_date[sizeof(record->expire_date) - 1] = '\0';
        record->current_point = (short)atoi(values[3]);
        record->leaf_point = (short)atoi(values[4]);
        record->day_water_count = (short)atoi(values[5]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacHousingTreeInfo_Update(DBConnectionManager* manager, const CharacHousingTreeInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_tree_info SET "
        "tree_id = %u, expire_date = '%s', current_point = %d, "
        "leaf_point = %d, day_water_count = %d "
        "WHERE charac_no = %u",
        record->tree_id, record->expire_date, record->current_point,
        record->leaf_point, record->day_water_count, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_housing_tree_info WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_housing_tree_info WHERE charac_no = %u", charac_no);

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

/* ==================== 树信息操作实现 ==================== */

int CharacHousingTreeInfo_UpdatePoints(DBConnectionManager* manager, unsigned int charac_no, short points) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_tree_info SET current_point = current_point + %d WHERE charac_no = %u",
        points, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_UpdateLeafPoints(DBConnectionManager* manager, unsigned int charac_no, short points) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_tree_info SET leaf_point = leaf_point + %d WHERE charac_no = %u",
        points, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_IncrementWaterCount(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_tree_info SET day_water_count = day_water_count + 1 WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_ResetWaterCount(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_tree_info SET day_water_count = 0 WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeInfo_UpdateExpireDate(DBConnectionManager* manager, unsigned int charac_no, const char* expire_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !expire_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_tree_info SET expire_date = '%s' WHERE charac_no = %u",
        expire_date, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacHousingTreeInfo_CountByTreeId(DBConnectionManager* manager, unsigned int tree_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_housing_tree_info WHERE tree_id = %u", tree_id);

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

int CharacHousingTreeInfo_IsExpired(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT expire_date < NOW() FROM charac_housing_tree_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int expired = atoi(values[0]);
        DBQueryResult_Free(&result);
        return expired;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacHousingTreeInfo_GetExpiringSoon(DBConnectionManager* manager, int days,
                                           unsigned int* charac_nos, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int count = 0;

    if (!manager || !charac_nos || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_housing_tree_info "
        "WHERE expire_date BETWEEN NOW() AND DATE_ADD(NOW(), INTERVAL %d DAY)",
        days);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        charac_nos[count++] = (unsigned int)atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void CharacHousingTreeInfo_PrintInfo(const CharacHousingTreeInfo* record) {
    if (!record) {
        printf("CharacHousingTreeInfo: NULL\n");
        return;
    }

    printf("CharacHousingTreeInfo {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  tree_id: %u\n", record->tree_id);
    printf("  expire_date: %s\n", record->expire_date);
    printf("  current_point: %d\n", record->current_point);
    printf("  leaf_point: %d\n", record->leaf_point);
    printf("  day_water_count: %d\n", record->day_water_count);
    printf("}\n");
}
