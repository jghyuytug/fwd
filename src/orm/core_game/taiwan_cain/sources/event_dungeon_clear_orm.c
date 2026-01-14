#include "event_dungeon_clear_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 3

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新活动副本通关记录
 */
int EventDungeonClear_Add(DBConnectionManager* manager, const EventDungeonClear* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO event_dungeon_clear (charac_no, clear_cnt, update_time) "
        "VALUES (%u, %u, '%s')",
        record->charac_no, record->clear_cnt, record->update_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 获取活动副本通关记录
 */
int EventDungeonClear_Get(DBConnectionManager* manager, unsigned int charac_no,
                          EventDungeonClear* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(EventDungeonClear));

    snprintf(query, sizeof(query),
        "SELECT charac_no, clear_cnt, update_time FROM event_dungeon_clear WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
    record->clear_cnt = values[1] ? (unsigned int)atoi(values[1]) : 0;
    if (values[2]) {
        strncpy(record->update_time, values[2], sizeof(record->update_time) - 1);
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 更新活动副本通关记录
 */
int EventDungeonClear_Update(DBConnectionManager* manager, const EventDungeonClear* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_dungeon_clear SET clear_cnt = %u, update_time = '%s' "
        "WHERE charac_no = %u",
        record->clear_cnt, record->update_time, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 删除活动副本通关记录
 */
int EventDungeonClear_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM event_dungeon_clear WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 检查活动副本通关记录是否存在
 */
int EventDungeonClear_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int exists = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_dungeon_clear WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        exists = atoi(values[0]) > 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

/**
 * 增加通关次数
 */
int EventDungeonClear_IncrementClearCount(DBConnectionManager* manager, unsigned int charac_no,
                                          unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_dungeon_clear SET clear_cnt = clear_cnt + %u, update_time = NOW() "
        "WHERE charac_no = %u",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 更新通关次数
 */
int EventDungeonClear_UpdateClearCount(DBConnectionManager* manager, unsigned int charac_no,
                                       unsigned int clear_cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_dungeon_clear SET clear_cnt = %u, update_time = NOW() "
        "WHERE charac_no = %u",
        clear_cnt, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 重置通关次数为0
 */
int EventDungeonClear_ResetClearCount(DBConnectionManager* manager, unsigned int charac_no) {
    return EventDungeonClear_UpdateClearCount(manager, charac_no, 0);
}

/**
 * 批量重置所有角色的通关次数
 */
int EventDungeonClear_BatchResetClearCounts(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_dungeon_clear SET clear_cnt = 0, update_time = NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

/**
 * 按通关次数精确查询
 */
int EventDungeonClear_GetByClearCount(DBConnectionManager* manager, unsigned int clear_cnt,
                                      EventDungeonClear* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, clear_cnt, update_time FROM event_dungeon_clear "
        "WHERE clear_cnt = %u LIMIT %d",
        clear_cnt, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventDungeonClear));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].clear_cnt = values[1] ? (unsigned int)atoi(values[1]) : 0;
        if (values[2]) {
            strncpy(records[count].update_time, values[2], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按最小通关次数查询（用于排行榜）
 */
int EventDungeonClear_GetByMinClearCount(DBConnectionManager* manager, unsigned int min_clear_cnt,
                                         EventDungeonClear* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, clear_cnt, update_time FROM event_dungeon_clear "
        "WHERE clear_cnt >= %u ORDER BY clear_cnt DESC LIMIT %d",
        min_clear_cnt, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventDungeonClear));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].clear_cnt = values[1] ? (unsigned int)atoi(values[1]) : 0;
        if (values[2]) {
            strncpy(records[count].update_time, values[2], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 获取通关次数TOP N
 */
int EventDungeonClear_GetTopByClearCount(DBConnectionManager* manager, int top_n,
                                         EventDungeonClear* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;
    int limit = (top_n < max_result) ? top_n : max_result;

    if (!manager || !records || max_result <= 0 || top_n <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, clear_cnt, update_time FROM event_dungeon_clear "
        "ORDER BY clear_cnt DESC LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventDungeonClear));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].clear_cnt = values[1] ? (unsigned int)atoi(values[1]) : 0;
        if (values[2]) {
            strncpy(records[count].update_time, values[2], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按更新时间查询
 */
int EventDungeonClear_GetByUpdatedAfter(DBConnectionManager* manager, const char* after_time,
                                        EventDungeonClear* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !after_time || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, clear_cnt, update_time FROM event_dungeon_clear "
        "WHERE update_time > '%s' LIMIT %d",
        after_time, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventDungeonClear));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].clear_cnt = values[1] ? (unsigned int)atoi(values[1]) : 0;
        if (values[2]) {
            strncpy(records[count].update_time, values[2], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 统计指定通关次数的角色数量
 */
int EventDungeonClear_CountByClearCount(DBConnectionManager* manager, unsigned int clear_cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_dungeon_clear WHERE clear_cnt = %u",
        clear_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 获取所有角色总通关次数
 */
unsigned long long EventDungeonClear_GetTotalClearCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    unsigned long long total = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(clear_cnt) FROM event_dungeon_clear");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = strtoull(values[0], NULL, 10);
    }

    DBQueryResult_Free(&result);
    return total;
}

/* ==================== 工具函数 ==================== */

/**
 * 打印活动副本通关记录信息
 */
void EventDungeonClear_PrintInfo(const EventDungeonClear* record) {
    if (!record) {
        return;
    }

    printf("========================================\n");
    printf("活动副本通关记录\n");
    printf("========================================\n");
    printf("角色编号: %u\n", record->charac_no);
    printf("通关次数: %u\n", record->clear_cnt);
    printf("更新时间: %s\n", record->update_time);
    printf("========================================\n");
}
