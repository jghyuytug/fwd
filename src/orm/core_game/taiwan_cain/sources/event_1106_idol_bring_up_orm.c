#include "event_1106_idol_bring_up_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== CRUD 操作 ==================== */

int Event1106IdolBringUp_Add(DBConnectionManager* manager, const Event1106IdolBringUp* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "REPLACE INTO event_1106_idol_bring_up (charac_no, m_id, pot_type, water_cnt, give_title_flag, occ_date, give_title_flag2) "
        "VALUES (%u, %u, %d, %d, %d, '%s', %d)",
        record->charac_no, record->m_id, (int)record->pot_type, (int)record->water_cnt,
        (int)record->give_title_flag, record->occ_date, (int)record->give_title_flag2);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_Get(DBConnectionManager* manager, unsigned int charac_no, Event1106IdolBringUp* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT charac_no, m_id, pot_type, water_cnt, give_title_flag, occ_date, give_title_flag2 "
        "FROM event_1106_idol_bring_up WHERE charac_no = %u", charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[7];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->m_id = (unsigned int)atoi(values[1]);
        record->pot_type = (signed char)atoi(values[2]);
        record->water_cnt = (signed char)atoi(values[3]);
        record->give_title_flag = (signed char)atoi(values[4]);
        strncpy(record->occ_date, values[5], sizeof(record->occ_date) - 1);
        record->occ_date[sizeof(record->occ_date) - 1] = '\0';
        record->give_title_flag2 = (signed char)atoi(values[6]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int Event1106IdolBringUp_Update(DBConnectionManager* manager, const Event1106IdolBringUp* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE event_1106_idol_bring_up SET m_id = %u, pot_type = %d, water_cnt = %d, "
        "give_title_flag = %d, occ_date = '%s', give_title_flag2 = %d WHERE charac_no = %u",
        record->m_id, (int)record->pot_type, (int)record->water_cnt,
        (int)record->give_title_flag, record->occ_date, (int)record->give_title_flag2, record->charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM event_1106_idol_bring_up WHERE charac_no = %u", charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_1106_idol_bring_up WHERE charac_no = %u", charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int Event1106IdolBringUp_IncrementWaterCount(DBConnectionManager* manager, unsigned int charac_no, int increment) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE event_1106_idol_bring_up SET water_cnt = water_cnt + %d WHERE charac_no = %u",
        increment, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_UpdatePotType(DBConnectionManager* manager, unsigned int charac_no, signed char pot_type) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE event_1106_idol_bring_up SET pot_type = %d WHERE charac_no = %u",
        (int)pot_type, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_MarkTitleGiven(DBConnectionManager* manager, unsigned int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE event_1106_idol_bring_up SET give_title_flag = 1 WHERE charac_no = %u",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_MarkTitle2Given(DBConnectionManager* manager, unsigned int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE event_1106_idol_bring_up SET give_title_flag2 = 1 WHERE charac_no = %u",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_ResetProgress(DBConnectionManager* manager, unsigned int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE event_1106_idol_bring_up SET pot_type = 0, water_cnt = 0, "
        "give_title_flag = 0, give_title_flag2 = 0 WHERE charac_no = %u",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1106IdolBringUp_DeleteOlderThan(DBConnectionManager* manager, int days) {
    if (!manager || days <= 0) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM event_1106_idol_bring_up WHERE occ_date < DATE_SUB(NOW(), INTERVAL %d DAY)",
        days);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int deleted = result.row_count;
    DBQueryResult_Free(&result);
    return deleted;
}

/* ==================== 查询操作 ==================== */

int Event1106IdolBringUp_CountAll(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM event_1106_idol_bring_up");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int Event1106IdolBringUp_CountTitleReceivers(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_1106_idol_bring_up WHERE give_title_flag = 1");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int Event1106IdolBringUp_CountTitle2Receivers(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_1106_idol_bring_up WHERE give_title_flag2 = 1");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int Event1106IdolBringUp_CountByPotType(DBConnectionManager* manager, signed char pot_type) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_1106_idol_bring_up WHERE pot_type = %d", (int)pot_type);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int Event1106IdolBringUp_GetTopWaterers(DBConnectionManager* manager, int top_n, Event1106IdolBringUp* records) {
    if (!manager || !records || top_n <= 0 || top_n > 100) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT charac_no, m_id, pot_type, water_cnt, give_title_flag, occ_date, give_title_flag2 "
        "FROM event_1106_idol_bring_up ORDER BY water_cnt DESC LIMIT %d", top_n);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[7];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < top_n) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].m_id = (unsigned int)atoi(values[1]);
        records[count].pot_type = (signed char)atoi(values[2]);
        records[count].water_cnt = (signed char)atoi(values[3]);
        records[count].give_title_flag = (signed char)atoi(values[4]);
        strncpy(records[count].occ_date, values[5], sizeof(records[count].occ_date) - 1);
        records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        records[count].give_title_flag2 = (signed char)atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int Event1106IdolBringUp_GetAvgWaterCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT AVG(water_cnt) FROM event_1106_idol_bring_up");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int avg = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        avg = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return avg;
}

int Event1106IdolBringUp_CountNoTitleReceived(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_1106_idol_bring_up WHERE give_title_flag = 0 AND give_title_flag2 = 0");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void Event1106IdolBringUp_PrintInfo(const Event1106IdolBringUp* record) {
    if (!record) {
        printf("  [Event1106IdolBringUp] NULL record\n");
        return;
    }

    printf("  ----------------------------------------\n");
    printf("  角色编号: %u\n", record->charac_no);
    printf("  偶像ID: %u\n", record->m_id);
    printf("  花盆类型: %d\n", (int)record->pot_type);
    printf("  浇水次数: %d\n", (int)record->water_cnt);
    printf("  第一阶段称号: %s\n", record->give_title_flag ? "已发放" : "未发放");
    printf("  活动参与时间: %s\n", record->occ_date);
    printf("  第二阶段称号: %s\n", record->give_title_flag2 ? "已发放" : "未发放");
    printf("  ----------------------------------------\n");
}
