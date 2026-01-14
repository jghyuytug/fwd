#include "charac_tower_despair_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* ==================== CRUD 操作实现 ==================== */

int CharacTowerDespair_Add(DBConnectionManager* manager, const CharacTowerDespair* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_tower_despair "
        "(charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date) "
        "VALUES (%d, '%s', %u, %u, %d, '%s', '%s')",
        record->charac_no,
        record->first_layer_start_date,
        (unsigned int)record->today_enter_count,
        (unsigned int)record->last_clear_layer,
        record->enter_count_by_week,
        record->m_date,
        record->last_clear_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_Get(DBConnectionManager* manager, int charac_no, CharacTowerDespair* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTowerDespair));

    snprintf(query, sizeof(query),
        "SELECT charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date "
        "FROM charac_tower_despair WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        if (values[1]) {
            strncpy(record->first_layer_start_date, values[1], sizeof(record->first_layer_start_date) - 1);
        }
        record->today_enter_count = (unsigned char)atoi(values[2]);
        record->last_clear_layer = (unsigned char)atoi(values[3]);
        record->enter_count_by_week = atoi(values[4]);
        if (values[5]) {
            strncpy(record->m_date, values[5], sizeof(record->m_date) - 1);
        }
        if (values[6]) {
            strncpy(record->last_clear_date, values[6], sizeof(record->last_clear_date) - 1);
        }

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerDespair_Update(DBConnectionManager* manager, const CharacTowerDespair* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET "
        "first_layer_start_date = '%s', today_enter_count = %u, last_clear_layer = %u, "
        "enter_count_by_week = %d, m_date = '%s', last_clear_date = '%s' "
        "WHERE charac_no = %d",
        record->first_layer_start_date,
        (unsigned int)record->today_enter_count,
        (unsigned int)record->last_clear_layer,
        record->enter_count_by_week,
        record->m_date,
        record->last_clear_date,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_Delete(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_despair WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_Exists(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_despair WHERE charac_no = %d", charac_no);

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

/* ==================== 专项操作实现 ==================== */

int CharacTowerDespair_IncrementTodayEnterCount(DBConnectionManager* manager, int charac_no, int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET today_enter_count = today_enter_count + %d WHERE charac_no = %d",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_ResetTodayEnterCount(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET today_enter_count = 0 WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_UpdateLastClearLayer(DBConnectionManager* manager, int charac_no, unsigned char layer) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET last_clear_layer = %u WHERE charac_no = %d",
        (unsigned int)layer, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_IncrementWeekEnterCount(DBConnectionManager* manager, int charac_no, int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET enter_count_by_week = enter_count_by_week + %d WHERE charac_no = %d",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_ResetWeekEnterCount(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET enter_count_by_week = 0 WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_UpdateLastClearDate(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET last_clear_date = NOW() WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespair_UpdateMDate(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair SET m_date = NOW() WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacTowerDespair_GetTotalCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_despair");

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

int CharacTowerDespair_GetByClearLayerRange(DBConnectionManager* manager, unsigned char min_layer, unsigned char max_layer,
                                              int* charac_nos, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_tower_despair WHERE last_clear_layer BETWEEN %u AND %u LIMIT %d",
        (unsigned int)min_layer, (unsigned int)max_layer, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        charac_nos[count++] = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerDespair_GetTopByLayer(DBConnectionManager* manager, int limit, int* charac_nos) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_tower_despair ORDER BY last_clear_layer DESC LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        charac_nos[count++] = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerDespair_GetTopByTodayCount(DBConnectionManager* manager, int limit, int* charac_nos) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_tower_despair ORDER BY today_enter_count DESC LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        charac_nos[count++] = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void CharacTowerDespair_PrintInfo(const CharacTowerDespair* record) {
    if (!record) {
        printf("CharacTowerDespair: NULL\n");
        return;
    }

    printf("CharacTowerDespair {\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  first_layer_start_date: %s\n", record->first_layer_start_date);
    printf("  today_enter_count: %u\n", (unsigned int)record->today_enter_count);
    printf("  last_clear_layer: %u\n", (unsigned int)record->last_clear_layer);
    printf("  enter_count_by_week: %d\n", record->enter_count_by_week);
    printf("  m_date: %s\n", record->m_date);
    printf("  last_clear_date: %s\n", record->last_clear_date);
    printf("}\n");
}
