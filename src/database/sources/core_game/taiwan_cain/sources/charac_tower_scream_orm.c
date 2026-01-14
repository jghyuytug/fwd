#include "charac_tower_scream_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作 ==================== */

int CharacTowerScream_Add(DBConnectionManager* manager, const CharacTowerScream* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_tower_scream "
        "(charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date) "
        "VALUES (%d, '%s', %u, %u, %d, '%s', '%s')",
        record->charac_no, record->first_layer_start_date,
        (unsigned int)record->today_enter_count, (unsigned int)record->last_clear_layer,
        record->enter_count_by_week, record->m_date, record->last_clear_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_Get(DBConnectionManager* manager, int charac_no, CharacTowerScream* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTowerScream));

    snprintf(query, sizeof(query),
        "SELECT charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date "
        "FROM charac_tower_scream WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        if (values[1]) strncpy(record->first_layer_start_date, values[1], sizeof(record->first_layer_start_date) - 1);
        record->today_enter_count = (unsigned char)atoi(values[2]);
        record->last_clear_layer = (unsigned char)atoi(values[3]);
        record->enter_count_by_week = atoi(values[4]);
        if (values[5]) strncpy(record->m_date, values[5], sizeof(record->m_date) - 1);
        if (values[6]) strncpy(record->last_clear_date, values[6], sizeof(record->last_clear_date) - 1);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerScream_Update(DBConnectionManager* manager, const CharacTowerScream* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET "
        "first_layer_start_date = '%s', today_enter_count = %u, last_clear_layer = %u, "
        "enter_count_by_week = %d, m_date = '%s', last_clear_date = '%s' "
        "WHERE charac_no = %d",
        record->first_layer_start_date, (unsigned int)record->today_enter_count,
        (unsigned int)record->last_clear_layer, record->enter_count_by_week,
        record->m_date, record->last_clear_date, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_Delete(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_scream WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_Exists(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_scream WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    int exists = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int CharacTowerScream_UpdateTodayEnterCount(DBConnectionManager* manager, int charac_no, unsigned char count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET today_enter_count = %u, m_date = NOW() "
        "WHERE charac_no = %d",
        (unsigned int)count, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_IncrementTodayEnterCount(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET today_enter_count = today_enter_count + 1, m_date = NOW() "
        "WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_ResetTodayEnterCount(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET today_enter_count = 0, m_date = NOW() "
        "WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_UpdateLastClearLayer(DBConnectionManager* manager, int charac_no, unsigned char layer) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET last_clear_layer = %u, last_clear_date = NOW(), m_date = NOW() "
        "WHERE charac_no = %d",
        (unsigned int)layer, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_UpdateEnterCountByWeek(DBConnectionManager* manager, int charac_no, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET enter_count_by_week = %d, m_date = NOW() "
        "WHERE charac_no = %d",
        count, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_IncrementEnterCountByWeek(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET enter_count_by_week = enter_count_by_week + 1, m_date = NOW() "
        "WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_ResetEnterCountByWeek(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET enter_count_by_week = 0, m_date = NOW() "
        "WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_UpdateLastClearDate(DBConnectionManager* manager, int charac_no, const char* clear_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !clear_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET last_clear_date = '%s', m_date = NOW() "
        "WHERE charac_no = %d",
        clear_date, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int CharacTowerScream_GetByMinLayer(DBConnectionManager* manager, unsigned char min_layer,
                                     CharacTowerScream* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date "
        "FROM charac_tower_scream WHERE last_clear_layer >= %u ORDER BY last_clear_layer DESC LIMIT %d",
        (unsigned int)min_layer, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].charac_no = atoi(values[0]);
        if (values[1]) strncpy(records[count].first_layer_start_date, values[1], sizeof(records[count].first_layer_start_date) - 1);
        records[count].today_enter_count = (unsigned char)atoi(values[2]);
        records[count].last_clear_layer = (unsigned char)atoi(values[3]);
        records[count].enter_count_by_week = atoi(values[4]);
        if (values[5]) strncpy(records[count].m_date, values[5], sizeof(records[count].m_date) - 1);
        if (values[6]) strncpy(records[count].last_clear_date, values[6], sizeof(records[count].last_clear_date) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerScream_GetTopByTodayEnterCount(DBConnectionManager* manager, int limit,
                                                CharacTowerScream* records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !records || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date "
        "FROM charac_tower_scream WHERE today_enter_count > 0 "
        "ORDER BY today_enter_count DESC LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        records[count].charac_no = atoi(values[0]);
        if (values[1]) strncpy(records[count].first_layer_start_date, values[1], sizeof(records[count].first_layer_start_date) - 1);
        records[count].today_enter_count = (unsigned char)atoi(values[2]);
        records[count].last_clear_layer = (unsigned char)atoi(values[3]);
        records[count].enter_count_by_week = atoi(values[4]);
        if (values[5]) strncpy(records[count].m_date, values[5], sizeof(records[count].m_date) - 1);
        if (values[6]) strncpy(records[count].last_clear_date, values[6], sizeof(records[count].last_clear_date) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerScream_GetTopByWeekEnterCount(DBConnectionManager* manager, int limit,
                                               CharacTowerScream* records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !records || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date "
        "FROM charac_tower_scream WHERE enter_count_by_week > 0 "
        "ORDER BY enter_count_by_week DESC LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        records[count].charac_no = atoi(values[0]);
        if (values[1]) strncpy(records[count].first_layer_start_date, values[1], sizeof(records[count].first_layer_start_date) - 1);
        records[count].today_enter_count = (unsigned char)atoi(values[2]);
        records[count].last_clear_layer = (unsigned char)atoi(values[3]);
        records[count].enter_count_by_week = atoi(values[4]);
        if (values[5]) strncpy(records[count].m_date, values[5], sizeof(records[count].m_date) - 1);
        if (values[6]) strncpy(records[count].last_clear_date, values[6], sizeof(records[count].last_clear_date) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerScream_GetByModifiedAfter(DBConnectionManager* manager, const char* date,
                                           CharacTowerScream* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !date || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, first_layer_start_date, today_enter_count, last_clear_layer, "
        "enter_count_by_week, m_date, last_clear_date "
        "FROM charac_tower_scream WHERE m_date >= '%s' ORDER BY m_date DESC LIMIT %d",
        date, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].charac_no = atoi(values[0]);
        if (values[1]) strncpy(records[count].first_layer_start_date, values[1], sizeof(records[count].first_layer_start_date) - 1);
        records[count].today_enter_count = (unsigned char)atoi(values[2]);
        records[count].last_clear_layer = (unsigned char)atoi(values[3]);
        records[count].enter_count_by_week = atoi(values[4]);
        if (values[5]) strncpy(records[count].m_date, values[5], sizeof(records[count].m_date) - 1);
        if (values[6]) strncpy(records[count].last_clear_date, values[6], sizeof(records[count].last_clear_date) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerScream_BatchResetTodayEnterCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET today_enter_count = 0, m_date = NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerScream_BatchResetWeekEnterCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_scream SET enter_count_by_week = 0, m_date = NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数 ==================== */

void CharacTowerScream_PrintInfo(const CharacTowerScream* record) {
    if (!record) {
        return;
    }

    printf("\nCharacTowerScream {\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  first_layer_start_date: %s\n", record->first_layer_start_date);
    printf("  today_enter_count: %u\n", (unsigned int)record->today_enter_count);
    printf("  last_clear_layer: %u\n", (unsigned int)record->last_clear_layer);
    printf("  enter_count_by_week: %d\n", record->enter_count_by_week);
    printf("  m_date: %s\n", record->m_date);
    printf("  last_clear_date: %s\n", record->last_clear_date);
    printf("}\n");
}
