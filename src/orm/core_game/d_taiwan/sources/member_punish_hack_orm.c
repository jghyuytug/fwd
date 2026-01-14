/**
 * DNF游戏服务器 - MemberPunishHack表ORM实现 (修复版)
 * 使用正确的DBConnectionManager_ExecuteQuery API
 */

#define _GNU_SOURCE
#include "../../include/database/headers/member_punish_hack_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========================================================================
 * 内部辅助函数
 * ======================================================================== */

/**
 * 从查询结果行填充黑客处罚信息结构
 */
static void FillHackFromRow(char** row, MemberPunishHack* hack) {
    hack->m_id = (unsigned int)atoi(row[0]);
    hack->occ_time = (unsigned int)atoi(row[1]);
    hack->period = (unsigned int)atoi(row[2]);
    hack->now_flag = atoi(row[3]);
    hack->auto_flag = atoi(row[4]);
    strncpy(hack->reason, row[5] ? row[5] : "", sizeof(hack->reason) - 1);
    hack->reason[sizeof(hack->reason) - 1] = '\0';
    strncpy(hack->hack_ip, row[6] ? row[6] : "", sizeof(hack->hack_ip) - 1);
    hack->hack_ip[sizeof(hack->hack_ip) - 1] = '\0';
}

/**
 * 转义字符串以防止SQL注入
 */
static void EscapeString(const char* src, char* dest, size_t dest_size) {
    size_t i, j;
    i = 0;
    j = 0;
    while (src[i] && j < dest_size - 2) {
        if (src[i] == '\'') {
            if (j < dest_size - 3) {
                dest[j++] = '\'';
                dest[j++] = '\'';
            }
        } else {
            dest[j++] = src[i];
        }
        i++;
    }
    dest[j] = '\0';
}

/* ========================================================================
 * 核心CRUD操作
 * ======================================================================== */

int MemberPunishHack_Add(DBConnectionManager* manager, MemberPunishHack* hack) {
    char query[2048];
    char escaped_reason[512];
    char escaped_ip[32];

    if (!manager || !hack) {
        return -1;
    }

    EscapeString(hack->reason, escaped_reason, sizeof(escaped_reason));
    EscapeString(hack->hack_ip, escaped_ip, sizeof(escaped_ip));

    snprintf(query, sizeof(query),
             "REPLACE INTO member_punish_hack "
             "(m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip) "
             "VALUES (%u, %u, %u, %d, %d, '%s', '%s')",
             hack->m_id, hack->occ_time, hack->period, hack->now_flag,
             hack->auto_flag, escaped_reason, escaped_ip);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret > 0) {
        printf("[MemberPunishHack ORM] Added hack punish: m_id=%u\n", hack->m_id);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHack_Get(DBConnectionManager* manager, unsigned int m_id,
                         MemberPunishHack* hack) {
    char query[512];

    if (!manager || !hack) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip "
             "FROM member_punish_hack WHERE m_id=%u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[7];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillHackFromRow(values, hack);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int MemberPunishHack_Update(DBConnectionManager* manager, MemberPunishHack* hack) {
    char query[2048];
    char escaped_reason[512];
    char escaped_ip[32];

    if (!manager || !hack) {
        return -1;
    }

    EscapeString(hack->reason, escaped_reason, sizeof(escaped_reason));
    EscapeString(hack->hack_ip, escaped_ip, sizeof(escaped_ip));

    snprintf(query, sizeof(query),
             "UPDATE member_punish_hack SET "
             "occ_time=%u, period=%u, now_flag=%d, auto_flag=%d, "
             "reason='%s', hack_ip='%s' WHERE m_id=%u",
             hack->occ_time, hack->period, hack->now_flag, hack->auto_flag,
             escaped_reason, escaped_ip, hack->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHack_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[256];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM member_punish_hack WHERE m_id=%u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    DBQueryResult_Free(&result);
    return (ret < 0) ? -1 : 0;
}

bool MemberPunishHack_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[256];

    if (!manager) {
        return false;
    }

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_punish_hack WHERE m_id=%u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    bool exists = false;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return exists;
}

int MemberPunishHack_GetAll(DBConnectionManager* manager,
                             MemberPunishHack* hacks, int offset, int limit) {
    char query[512];

    if (!manager || !hacks || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip "
             "FROM member_punish_hack ORDER BY occ_time DESC LIMIT %d OFFSET %d",
             limit, offset);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = (result.row_count < limit) ? result.row_count : limit;
    int i;
    char* values[7];
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillHackFromRow(values, &hacks[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ========================================================================
 * 业务逻辑查询函数
 * ======================================================================== */

bool MemberPunishHack_HasActiveHackPunish(DBConnectionManager* manager, unsigned int m_id) {
    char query[512];

    if (!manager) {
        return false;
    }

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_punish_hack "
             "WHERE m_id=%u AND now_flag=%d AND (occ_time + period) > UNIX_TIMESTAMP()",
             m_id, HACK_FLAG_ACTIVE);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    bool has_active = false;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        has_active = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return has_active;
}

int MemberPunishHack_GetByIP(DBConnectionManager* manager, const char* hack_ip,
                              MemberPunishHack* hacks, int max_count) {
    char query[512];
    char escaped_ip[32];

    if (!manager || !hack_ip || !hacks || max_count <= 0) {
        return -1;
    }

    EscapeString(hack_ip, escaped_ip, sizeof(escaped_ip));

    snprintf(query, sizeof(query),
             "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip "
             "FROM member_punish_hack WHERE hack_ip='%s' ORDER BY occ_time DESC LIMIT %d",
             escaped_ip, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;
    int i;
    char* values[7];
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillHackFromRow(values, &hacks[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPunishHack_GetAutoFlag(DBConnectionManager* manager,
                                  MemberPunishHack* hacks, int offset, int limit) {
    char query[512];

    if (!manager || !hacks || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip "
             "FROM member_punish_hack WHERE auto_flag=%d ORDER BY occ_time DESC LIMIT %d OFFSET %d",
             HACK_AUTO_SYSTEM, limit, offset);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = (result.row_count < limit) ? result.row_count : limit;
    int i;
    char* values[7];
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillHackFromRow(values, &hacks[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPunishHack_GetManualFlag(DBConnectionManager* manager,
                                    MemberPunishHack* hacks, int offset, int limit) {
    char query[512];

    if (!manager || !hacks || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip "
             "FROM member_punish_hack WHERE auto_flag=%d ORDER BY occ_time DESC LIMIT %d OFFSET %d",
             HACK_AUTO_MANUAL, limit, offset);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = (result.row_count < limit) ? result.row_count : limit;
    int i;
    char* values[7];
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillHackFromRow(values, &hacks[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPunishHack_UpdateNowFlag(DBConnectionManager* manager, unsigned int m_id, int now_flag) {
    char query[256];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "UPDATE member_punish_hack SET now_flag=%d WHERE m_id=%u",
             now_flag, m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    DBQueryResult_Free(&result);
    return (ret < 0) ? -1 : 0;
}

int MemberPunishHack_GetCount(DBConnectionManager* manager) {
    char query[256];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_punish_hack");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int count = -1;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

bool MemberPunishHack_IsExpired(DBConnectionManager* manager, unsigned int m_id) {
    char query[512];

    if (!manager) {
        return false;
    }

    snprintf(query, sizeof(query),
             "SELECT (occ_time + period) < UNIX_TIMESTAMP() AS expired "
             "FROM member_punish_hack WHERE m_id=%u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    bool expired = false;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        expired = (atoi(values[0]) == 1);
    }

    DBQueryResult_Free(&result);
    return expired;
}

int MemberPunishHack_GetExpiredRecords(DBConnectionManager* manager,
                                        MemberPunishHack* hacks, int max_count) {
    char query[512];

    if (!manager || !hacks || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip "
             "FROM member_punish_hack WHERE (occ_time + period) < UNIX_TIMESTAMP() "
             "ORDER BY occ_time DESC LIMIT %d", max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;
    int i;
    char* values[7];
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillHackFromRow(values, &hacks[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ========================================================================
 * 批量操作
 * ======================================================================== */

int MemberPunishHack_AddBatch(DBConnectionManager* manager, MemberPunishHack* hacks, int count) {
    int i;
    int success = 0;

    if (!manager || !hacks || count <= 0) {
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (MemberPunishHack_Add(manager, &hacks[i]) == 0) {
            success++;
        }
    }

    return success;
}

int MemberPunishHack_UpdateExpiredFlags(DBConnectionManager* manager) {
    char query[256];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "UPDATE member_punish_hack SET now_flag=%d "
             "WHERE (occ_time + period) < UNIX_TIMESTAMP() AND now_flag=%d",
             HACK_FLAG_INACTIVE, HACK_FLAG_ACTIVE);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

/* ========================================================================
 * 工具函数
 * ======================================================================== */

void MemberPunishHack_Init(MemberPunishHack* hack) {
    if (!hack) {
        return;
    }

    memset(hack, 0, sizeof(MemberPunishHack));
    hack->m_id = 0;
    hack->occ_time = (unsigned int)time(NULL);
    hack->period = 0;
    hack->now_flag = HACK_FLAG_ACTIVE;
    hack->auto_flag = HACK_AUTO_MANUAL;
    hack->reason[0] = '\0';
    hack->hack_ip[0] = '\0';
}

void MemberPunishHack_Print(const MemberPunishHack* hack) {
    if (!hack) {
        return;
    }

    printf("\n========================================\n");
    printf("黑客处罚记录信息\n");
    printf("========================================\n");
    printf("会员ID: %u\n", hack->m_id);
    printf("发生时间: %u\n", hack->occ_time);
    printf("封禁周期: %u 秒\n", hack->period);
    printf("当前状态: %s\n", MemberPunishHack_GetNowFlagName(hack->now_flag));
    printf("自动标志: %s\n", MemberPunishHack_GetAutoFlagName(hack->auto_flag));
    printf("处罚原因: %s\n", hack->reason);
    printf("黑客IP: %s\n", hack->hack_ip);
    printf("========================================\n\n");
}

const char* MemberPunishHack_GetNowFlagName(int now_flag) {
    switch (now_flag) {
        case HACK_FLAG_INACTIVE: return "未生效/已过期";
        case HACK_FLAG_ACTIVE:   return "生效中";
        default:                 return "未知状态";
    }
}

const char* MemberPunishHack_GetAutoFlagName(int auto_flag) {
    switch (auto_flag) {
        case HACK_AUTO_MANUAL: return "手动处罚";
        case HACK_AUTO_SYSTEM: return "系统自动";
        default:               return "未知类型";
    }
}

unsigned int MemberPunishHack_CalculateExpireTime(unsigned int period_seconds) {
    return (unsigned int)time(NULL) + period_seconds;
}
