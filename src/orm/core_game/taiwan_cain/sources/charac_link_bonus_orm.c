#include "charac_link_bonus_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 1024
#define MAX_FIELDS 16

/* ==================== CRUD 操作实现 ==================== */

int CharacLinkBonus_Add(DBConnectionManager* manager, const CharacLinkBonus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_link_bonus "
        "(charac_no, exp, gold, mercenary_start_time, mercenary_finish_time, mercenary_area, mercenary_period) "
        "VALUES (%u, %u, %u, %d, %d, %d, %d)",
        record->charac_no, record->exp, record->gold,
        record->mercenary_start_time, record->mercenary_finish_time,
        (int)record->mercenary_area, (int)record->mercenary_period);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_Get(DBConnectionManager* manager, unsigned int charac_no, CharacLinkBonus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacLinkBonus));

    snprintf(query, sizeof(query),
        "SELECT charac_no, exp, gold, mercenary_start_time, mercenary_finish_time, mercenary_area, mercenary_period "
        "FROM charac_link_bonus WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->exp = (unsigned int)atoi(values[1]);
        record->gold = (unsigned int)atoi(values[2]);
        record->mercenary_start_time = atoi(values[3]);
        record->mercenary_finish_time = atoi(values[4]);
        record->mercenary_area = (char)atoi(values[5]);
        record->mercenary_period = (char)atoi(values[6]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacLinkBonus_Update(DBConnectionManager* manager, const CharacLinkBonus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET "
        "exp = %u, gold = %u, mercenary_start_time = %d, mercenary_finish_time = %d, "
        "mercenary_area = %d, mercenary_period = %d "
        "WHERE charac_no = %u",
        record->exp, record->gold,
        record->mercenary_start_time, record->mercenary_finish_time,
        (int)record->mercenary_area, (int)record->mercenary_period,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_link_bonus WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_link_bonus WHERE charac_no = %u", charac_no);

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

int CharacLinkBonus_UpdateExp(DBConnectionManager* manager, unsigned int charac_no, unsigned int exp) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET exp = %u WHERE charac_no = %u",
        exp, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_AddExp(DBConnectionManager* manager, unsigned int charac_no, unsigned int exp) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET exp = exp + %u WHERE charac_no = %u",
        exp, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_UpdateGold(DBConnectionManager* manager, unsigned int charac_no, unsigned int gold) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET gold = %u WHERE charac_no = %u",
        gold, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_AddGold(DBConnectionManager* manager, unsigned int charac_no, unsigned int gold) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET gold = gold + %u WHERE charac_no = %u",
        gold, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_UpdateMercenaryInfo(DBConnectionManager* manager, unsigned int charac_no,
                                         int start_time, int finish_time, char area, char period) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET "
        "mercenary_start_time = %d, mercenary_finish_time = %d, "
        "mercenary_area = %d, mercenary_period = %d "
        "WHERE charac_no = %u",
        start_time, finish_time, (int)area, (int)period, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLinkBonus_IsMercenaryActive(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int current_time = (int)time(NULL);

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_link_bonus "
        "WHERE charac_no = %u AND mercenary_start_time <= %d AND mercenary_finish_time >= %d",
        charac_no, current_time, current_time);

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

int CharacLinkBonus_ClearMercenaryInfo(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_link_bonus SET "
        "mercenary_start_time = 0, mercenary_finish_time = 0, "
        "mercenary_area = -1, mercenary_period = -1 "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

unsigned long long CharacLinkBonus_GetTotalExp(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    unsigned long long total = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(exp) FROM charac_link_bonus");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = (unsigned long long)atoll(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

unsigned long long CharacLinkBonus_GetTotalGold(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    unsigned long long total = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(gold) FROM charac_link_bonus");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = (unsigned long long)atoll(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

int CharacLinkBonus_GetActiveMercenaryCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int current_time = (int)time(NULL);

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_link_bonus "
        "WHERE mercenary_start_time <= %d AND mercenary_finish_time >= %d",
        current_time, current_time);

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

/* ==================== 工具函数实现 ==================== */

void CharacLinkBonus_PrintInfo(const CharacLinkBonus* record) {
    if (!record) {
        printf("CharacLinkBonus: NULL\n");
        return;
    }

    printf("CharacLinkBonus {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  exp: %u\n", record->exp);
    printf("  gold: %u\n", record->gold);
    printf("  mercenary_start_time: %d\n", record->mercenary_start_time);
    printf("  mercenary_finish_time: %d\n", record->mercenary_finish_time);
    printf("  mercenary_area: %d\n", (int)record->mercenary_area);
    printf("  mercenary_period: %d\n", (int)record->mercenary_period);
    printf("}\n");
}
