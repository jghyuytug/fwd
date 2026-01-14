/**
 * 活动条件化信息 ORM 实现
 *
 * 表: event_conditionable_info
 * 数据库: taiwan_cain (DB_TYPE_CAIN = 2)
 */

#include "../headers/event_conditionable_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048
#define MAX_FIELDS 32

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新活动条件化信息
 */
int EventConditionableInfo_Add(DBConnectionManager* manager, const EventConditionableInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO event_conditionable_info "
        "(charac_no, current_step, reward_step, update_time) "
        "VALUES (%u, %u, %u, NOW())",
        record->charac_no,
        record->current_step,
        record->reward_step);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 获取活动条件化信息
 */
int EventConditionableInfo_Get(DBConnectionManager* manager, unsigned int charac_no,
                                     EventConditionableInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(EventConditionableInfo));

    snprintf(query, sizeof(query),
        "SELECT charac_no, current_step, reward_step, update_time "
        "FROM event_conditionable_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
    record->current_step = values[1] ? (unsigned char)atoi(values[1]) : 0;
    record->reward_step = values[2] ? (unsigned char)atoi(values[2]) : 0;
    if (values[3]) {
        strncpy(record->update_time, values[3], sizeof(record->update_time) - 1);
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 更新活动条件化信息
 */
int EventConditionableInfo_Update(DBConnectionManager* manager, const EventConditionableInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET "
        "current_step = %u, reward_step = %u, update_time = NOW() "
        "WHERE charac_no = %u",
        record->current_step,
        record->reward_step,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 删除活动条件化信息
 */
int EventConditionableInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM event_conditionable_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 检查活动条件化信息是否存在
 */
int EventConditionableInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_conditionable_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = values[0] ? atoi(values[0]) : 0;
    DBQueryResult_Free(&result);

    return count > 0 ? 1 : 0;
}

/* ==================== 专项操作 ==================== */

/**
 * 增加当前步骤
 */
int EventConditionableInfo_IncrementCurrentStep(DBConnectionManager* manager, unsigned int charac_no,
                                                      int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET current_step = current_step + %d, update_time = NOW() "
        "WHERE charac_no = %u",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 更新当前步骤
 */
int EventConditionableInfo_UpdateCurrentStep(DBConnectionManager* manager, unsigned int charac_no,
                                                   unsigned char current_step) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET current_step = %u, update_time = NOW() "
        "WHERE charac_no = %u",
        current_step, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 更新奖励步骤
 */
int EventConditionableInfo_UpdateRewardStep(DBConnectionManager* manager, unsigned int charac_no,
                                                  unsigned char reward_step) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET reward_step = %u, update_time = NOW() "
        "WHERE charac_no = %u",
        reward_step, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 重置进度（将current_step和reward_step都设为0）
 */
int EventConditionableInfo_ResetProgress(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET current_step = 0, reward_step = 0, update_time = NOW() "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 批量重置所有角色的进度
 */
int EventConditionableInfo_BatchResetProgress(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET current_step = 0, reward_step = 0, update_time = NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 领取奖励（将reward_step设置为current_step）
 */
int EventConditionableInfo_ClaimReward(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE event_conditionable_info SET reward_step = current_step, update_time = NOW() "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

/**
 * 按当前步骤精确查询
 */
int EventConditionableInfo_GetByCurrentStep(DBConnectionManager* manager, unsigned char current_step,
                                                  EventConditionableInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, current_step, reward_step, update_time "
        "FROM event_conditionable_info WHERE current_step = %u "
        "ORDER BY charac_no LIMIT %d",
        current_step, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventConditionableInfo));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].current_step = values[1] ? (unsigned char)atoi(values[1]) : 0;
        records[count].reward_step = values[2] ? (unsigned char)atoi(values[2]) : 0;
        if (values[3]) {
            strncpy(records[count].update_time, values[3], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按最小当前步骤查询（用于排行榜）
 */
int EventConditionableInfo_GetByMinCurrentStep(DBConnectionManager* manager, unsigned char min_current_step,
                                                     EventConditionableInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, current_step, reward_step, update_time "
        "FROM event_conditionable_info WHERE current_step >= %u "
        "ORDER BY current_step DESC, charac_no LIMIT %d",
        min_current_step, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventConditionableInfo));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].current_step = values[1] ? (unsigned char)atoi(values[1]) : 0;
        records[count].reward_step = values[2] ? (unsigned char)atoi(values[2]) : 0;
        if (values[3]) {
            strncpy(records[count].update_time, values[3], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 获取当前步骤TOP N
 */
int EventConditionableInfo_GetTopByCurrentStep(DBConnectionManager* manager, int top_n,
                                                     EventConditionableInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;
    int limit = (top_n < max_result) ? top_n : max_result;

    if (!manager || !records || max_result <= 0 || top_n <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, current_step, reward_step, update_time "
        "FROM event_conditionable_info "
        "ORDER BY current_step DESC, charac_no LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventConditionableInfo));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].current_step = values[1] ? (unsigned char)atoi(values[1]) : 0;
        records[count].reward_step = values[2] ? (unsigned char)atoi(values[2]) : 0;
        if (values[3]) {
            strncpy(records[count].update_time, values[3], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 查询未领取奖励的角色（current_step > reward_step）
 */
int EventConditionableInfo_GetUnclaimedRewards(DBConnectionManager* manager,
                                                     EventConditionableInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, current_step, reward_step, update_time "
        "FROM event_conditionable_info WHERE current_step > reward_step "
        "ORDER BY charac_no LIMIT %d",
        max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventConditionableInfo));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].current_step = values[1] ? (unsigned char)atoi(values[1]) : 0;
        records[count].reward_step = values[2] ? (unsigned char)atoi(values[2]) : 0;
        if (values[3]) {
            strncpy(records[count].update_time, values[3], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按更新时间查询
 */
int EventConditionableInfo_GetByUpdatedAfter(DBConnectionManager* manager, const char* after_time,
                                                   EventConditionableInfo* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !after_time || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, current_step, reward_step, update_time "
        "FROM event_conditionable_info WHERE update_time > '%s' "
        "ORDER BY update_time DESC LIMIT %d",
        after_time, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        memset(&records[count], 0, sizeof(EventConditionableInfo));
        records[count].charac_no = values[0] ? (unsigned int)atoi(values[0]) : 0;
        records[count].current_step = values[1] ? (unsigned char)atoi(values[1]) : 0;
        records[count].reward_step = values[2] ? (unsigned char)atoi(values[2]) : 0;
        if (values[3]) {
            strncpy(records[count].update_time, values[3], sizeof(records[count].update_time) - 1);
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 统计指定步骤的角色数量
 */
int EventConditionableInfo_CountByCurrentStep(DBConnectionManager* manager, unsigned char current_step) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_conditionable_info WHERE current_step = %u",
        current_step);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = values[0] ? atoi(values[0]) : 0;
    DBQueryResult_Free(&result);

    return count;
}

/**
 * 统计未领取奖励的角色数量
 */
int EventConditionableInfo_CountUnclaimedRewards(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_conditionable_info WHERE current_step > reward_step");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = values[0] ? atoi(values[0]) : 0;
    DBQueryResult_Free(&result);

    return count;
}

/**
 * 获取所有角色的平均步骤进度
 */
double EventConditionableInfo_GetAverageProgress(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1.0;
    }

    snprintf(query, sizeof(query),
        "SELECT AVG(current_step) FROM event_conditionable_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1.0;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1.0;
    }

    double avg = values[0] ? atof(values[0]) : 0.0;
    DBQueryResult_Free(&result);

    return avg;
}

/* ==================== 工具函数 ==================== */

/**
 * 打印活动条件化信息
 */
void EventConditionableInfo_PrintInfo(const EventConditionableInfo* record) {
    if (!record) {
        printf("记录为空\n");
        return;
    }

    printf("========================================\n");
    printf("角色编号: %u\n", record->charac_no);
    printf("当前步骤: %u\n", record->current_step);
    printf("奖励步骤: %u\n", record->reward_step);

    if (record->current_step > record->reward_step) {
        printf("奖励状态: 有未领取奖励 (差%u步)\n",
               record->current_step - record->reward_step);
    } else {
        printf("奖励状态: 已领取全部奖励\n");
    }

    printf("更新时间: %s\n", record->update_time);
    printf("========================================\n");
}
